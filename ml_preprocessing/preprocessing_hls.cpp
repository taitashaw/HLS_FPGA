#include "preprocessing_hls.hpp"
#include <hls_stream.h>
#include <cmath>
#include <hls_math.h>

constexpr float PI = 3.14159265358979323846f;

// Struct to hold all design parameters for dataflow streaming
struct config_t {
    int input_size;
    int window_size;
    int stride;
    int n_win;
    WindowType window_type;
    NormalizationType normalization;
};

//==============================================================
// Single Window Normalization (Inlined)
//==============================================================

void normalize_single_window_hls(
    data_t window[MAX_WINDOW_SIZE],
    int window_size,
    NormalizationType normalization)
{
#pragma HLS INLINE

    if (normalization == NormalizationType::NONE)
        return;

    if (normalization == NormalizationType::MIN_MAX)
    {
        const data_t EPSILON = 0.001f;
        data_t min_val = window[0];
        data_t max_val = window[0];

        MIN_MAX_LOOP: for (int i = 1; i < MAX_WINDOW_SIZE; i++)
        {
#pragma HLS PIPELINE II=1
            if (i < window_size) {
                if (window[i] < min_val) min_val = window[i];
                if (window[i] > max_val) max_val = window[i];
            }
        }

        data_t range = max_val - min_val;
        if (range < EPSILON) range = 1.0f;

        data_t inv_range = (data_t)1.0f / range;

        NORM_MINMAX_LOOP: for (int i = 0; i < MAX_WINDOW_SIZE; i++)
        {
#pragma HLS PIPELINE II=1
            if (i < window_size) {
                window[i] = (window[i] - min_val) * inv_range;
            }
        }
    }
    else if (normalization == NormalizationType::Z_SCORE)
    {
        const data_t EPSILON = 0.001f;
        typedef ap_fixed<40,16> accum_t;

        accum_t sum = 0;
        MEAN_LOOP: for (int i = 0; i < MAX_WINDOW_SIZE; i++)
        {
#pragma HLS PIPELINE II=1
            if (i < window_size)
                sum += (accum_t)window[i];
        }

        data_t mean = (data_t)(sum / window_size);

        accum_t var_sum = 0;
        VAR_LOOP: for (int i = 0; i < MAX_WINDOW_SIZE; i++)
        {
#pragma HLS PIPELINE II=1
            if (i < window_size) {
                data_t diff = window[i] - mean;
                var_sum += (accum_t)(diff * diff);
            }
        }

        data_t variance = (data_t)(var_sum / window_size);
        data_t stddev = hls::sqrt(variance);

        if (stddev < EPSILON) stddev = 1.0f;

        data_t inv_stddev = (data_t)1.0f / stddev;

        NORM_ZSCORE_LOOP: for (int i = 0; i < MAX_WINDOW_SIZE; i++)
        {
#pragma HLS PIPELINE II=1
            if (i < window_size) {
                window[i] = (window[i] - mean) * inv_stddev;
            }
        }
    }
}

//==============================================================
// Dataflow Task 1: Read Memory
//==============================================================

static void read_input_task(
    const data_t input[MAX_INPUT_SIZE],
    hls::stream<data_t>& in_fifo,
    hls::stream<config_t>& cfg_fifo_out,
    config_t cfg)
{
#pragma HLS INLINE off

    // Pass configuration to next streaming task
    cfg_fifo_out.write(cfg);

    READ_LOOP: for (int i = 0; i < MAX_INPUT_SIZE; i++) {
#pragma HLS PIPELINE II=1
        if (i < cfg.input_size) {
            in_fifo.write(input[i]);
        }
    }
}

//==============================================================
// Dataflow Task 2: Window Generator
//==============================================================

static void window_gen_task(
    hls::stream<data_t>& in_fifo,
    hls::stream<data_t>& window_fifo,
    hls::stream<config_t>& cfg_fifo_in,
    hls::stream<config_t>& cfg_fifo_out)
{
#pragma HLS INLINE off

    config_t cfg = cfg_fifo_in.read();
    cfg_fifo_out.write(cfg);

    data_t buf[MAX_INPUT_SIZE];

    BUF_LOAD: for (int i = 0; i < MAX_INPUT_SIZE; i++) {
#pragma HLS PIPELINE II=1
        if (i < cfg.input_size) {
            buf[i] = in_fifo.read();
        }
    }

    WIN_OUTER: for (int w = 0; w < MAX_WINDOWS; w++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_WINDOWS
        if (w < cfg.n_win) {
            int start = w * cfg.stride;
            WIN_INNER: for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
#pragma HLS PIPELINE II=1
                if (i < cfg.window_size) {
                    window_fifo.write(buf[start + i]);
                }
            }
        }
    }
}

//==============================================================
// Dataflow Task 3: Apply Window Function
//==============================================================

static void apply_window_task(
    hls::stream<data_t>& window_fifo,
    hls::stream<data_t>& coeff_fifo,
    hls::stream<config_t>& cfg_fifo_in,
    hls::stream<config_t>& cfg_fifo_out)
{
#pragma HLS INLINE off

    config_t cfg = cfg_fifo_in.read();
    cfg_fifo_out.write(cfg);

    data_t coeff[MAX_WINDOW_SIZE];
#pragma HLS ARRAY_PARTITION variable=coeff complete

    // Inline window coefficient generation
    if (cfg.window_size > 0) {
        if (cfg.window_size == 1) {
            coeff[0] = 1.0f;
        } else {
            GEN_COEFF_LOOP: for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
#pragma HLS PIPELINE II=1
                if (i < cfg.window_size) {
                    float angle = (2.0f * PI * static_cast<float>(i)) /
                                  static_cast<float>(cfg.window_size - 1);

                    switch (cfg.window_type) {
                        case WindowType::RECTANGULAR:
                            coeff[i] = 1.0f;
                            break;
                        case WindowType::HAMMING:
                            coeff[i] = 0.54f - 0.46f * hls::cos(angle);
                            break;
                        case WindowType::HANN:
                            coeff[i] = 0.5f * (1.0f - hls::cos(angle));
                            break;
                    }
                }
            }
        }
    }

    APPLY_OUTER: for (int w = 0; w < MAX_WINDOWS; w++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_WINDOWS
        if (w < cfg.n_win) {
            APPLY_INNER: for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
#pragma HLS PIPELINE II=1
                if (i < cfg.window_size) {
                    data_t sample = window_fifo.read();
                    coeff_fifo.write(sample * coeff[i]);
                }
            }
        }
    }
}

//==============================================================
// Dataflow Task 4: Normalize & Write Back
//==============================================================

static void normalize_store_task(
    hls::stream<data_t>& coeff_fifo,
    data_t output[MAX_WINDOWS][MAX_WINDOW_SIZE],
    hls::stream<config_t>& cfg_fifo_in)
{
#pragma HLS INLINE off

    config_t cfg = cfg_fifo_in.read();

    data_t window_buf[MAX_WINDOW_SIZE];
#pragma HLS ARRAY_PARTITION variable=window_buf complete

    NORM_OUTER: for (int w = 0; w < MAX_WINDOWS; w++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_WINDOWS
        if (w < cfg.n_win) {
            NORM_READ: for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
#pragma HLS PIPELINE II=1
                if (i < cfg.window_size) {
                    window_buf[i] = coeff_fifo.read();
                }
            }

            normalize_single_window_hls(window_buf, cfg.window_size, cfg.normalization);

            NORM_WRITE: for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
#pragma HLS PIPELINE II=1
                if (i < cfg.window_size) {
                    output[w][i] = window_buf[i];
                }
            }
        }
    }
}

//==============================================================
// CSIM Compatibility Stubs
//==============================================================

void generate_window_coefficients(
    data_t coeff[MAX_WINDOW_SIZE],
    int window_size,
    WindowType window_type)
{
    if (window_size <= 0) return;
    if (window_size == 1) { coeff[0] = 1.0f; return; }

    for (int i = 0; i < window_size; i++) {
        float angle = (2.0f * PI * static_cast<float>(i)) /
                      static_cast<float>(window_size - 1);
        switch (window_type) {
            case WindowType::RECTANGULAR: coeff[i] = 1.0f; break;
            case WindowType::HAMMING:     coeff[i] = 0.54f - 0.46f * hls::cos(angle); break;
            case WindowType::HANN:        coeff[i] = 0.5f * (1.0f - hls::cos(angle)); break;
        }
    }
}

void sliding_window_hls(
    const data_t input[MAX_INPUT_SIZE],
    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int input_size, int window_size, int stride, int &num_windows)
{
    num_windows = 0;
    if (window_size <= 0 || stride <= 0 || input_size < window_size) return;
    for (int start = 0; start + window_size <= input_size; start += stride) {
        for (int i = 0; i < window_size; i++) windows[num_windows][i] = input[start + i];
        num_windows++;
    }
}

void apply_window_hls(
    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int num_windows, int window_size, WindowType window_type)
{
    data_t coeff[MAX_WINDOW_SIZE];
    generate_window_coefficients(coeff, window_size, window_type);
    for (int w = 0; w < num_windows; w++) {
        for (int i = 0; i < window_size; i++) windows[w][i] *= coeff[i];
    }
}

void min_max_normalize_hls(data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE], int num_windows, int window_size) {
    for (int w = 0; w < num_windows; w++) normalize_single_window_hls(windows[w], window_size, NormalizationType::MIN_MAX);
}

void zscore_normalize_hls(data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE], int num_windows, int window_size) {
    for (int w = 0; w < num_windows; w++) normalize_single_window_hls(windows[w], window_size, NormalizationType::Z_SCORE);
}

void normalize_windows_hls(data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE], int num_windows, int window_size, NormalizationType normalization) {
    for (int w = 0; w < num_windows; w++) normalize_single_window_hls(windows[w], window_size, normalization);
}

//==============================================================
// Top-Level Dataflow Kernel
//==============================================================

void ml_preprocessing_hls(
    const data_t input[MAX_INPUT_SIZE],
    data_t output[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int input_size,
    int window_size,
    int stride,
    WindowType window_type,
    NormalizationType normalization,
    int &num_windows)
{
#pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem max_read_burst_length=64
#pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem max_write_burst_length=64

#pragma HLS INTERFACE s_axilite port=input_size
#pragma HLS INTERFACE s_axilite port=window_size
#pragma HLS INTERFACE s_axilite port=stride
#pragma HLS INTERFACE s_axilite port=window_type
#pragma HLS INTERFACE s_axilite port=normalization
#pragma HLS INTERFACE s_axilite port=num_windows
#pragma HLS INTERFACE s_axilite port=return

    if (input_size > MAX_INPUT_SIZE || input_size <= 0 ||
        window_size <= 0 || window_size > MAX_WINDOW_SIZE ||
        stride <= 0 || input_size < window_size)
    {
        num_windows = 0;
        return;
    }

    int n_win = ((input_size - window_size) / stride) + 1;
    num_windows = n_win;

    config_t cfg;
    cfg.input_size = input_size;
    cfg.window_size = window_size;
    cfg.stride = stride;
    cfg.n_win = n_win;
    cfg.window_type = window_type;
    cfg.normalization = normalization;

    hls::stream<data_t> in_fifo("in_fifo");
    hls::stream<data_t> window_fifo("window_fifo");
    hls::stream<data_t> coeff_fifo("coeff_fifo");

    hls::stream<config_t> cfg_c1("cfg_c1");
    hls::stream<config_t> cfg_c2("cfg_c2");
    hls::stream<config_t> cfg_c3("cfg_c3");

#pragma HLS STREAM variable=in_fifo depth=1024
#pragma HLS STREAM variable=window_fifo depth=64
#pragma HLS STREAM variable=coeff_fifo depth=64

#pragma HLS STREAM variable=cfg_c1 depth=2
#pragma HLS STREAM variable=cfg_c2 depth=2
#pragma HLS STREAM variable=cfg_c3 depth=2

#pragma HLS DATAFLOW

    read_input_task(input, in_fifo, cfg_c1, cfg);
    window_gen_task(in_fifo, window_fifo, cfg_c1, cfg_c2);
    apply_window_task(window_fifo, coeff_fifo, cfg_c2, cfg_c3);
    normalize_store_task(coeff_fifo, output, cfg_c3);
}

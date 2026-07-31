#include "preprocessing_hls.hpp"

#include <cmath>
#include <hls_math.h>

const data_t PI = 3.14159265358979323846f;

//==============================================================
// Precomputed Window Coefficients (Window Size = 4)
//==============================================================

static const data_t HAMMING_4[4] =
{
    0.08f,
    0.77f,
    0.77f,
    0.08f
};

static const data_t HANN_4[4] =
{
    0.00f,
    0.75f,
    0.75f,
    0.00f
};

//==============================================================
// Generate Window Coefficients
//==============================================================



static void generate_window_coefficients(
    data_t coeff[MAX_WINDOW_SIZE],
    int window_size,
    WindowType window_type)
{

#pragma HLS INLINE

    for(int i = 0; i < window_size; i++)
    {

#pragma HLS PIPELINE II=1

        switch(window_type)
        {

        case WindowType::RECTANGULAR:
            coeff[i] = 1.0f;
            break;

        case WindowType::HAMMING:
            coeff[i] = HAMMING_4[i];
            break;

        case WindowType::HANN:
            coeff[i] = HANN_4[i];
            break;
        }
    }
}



//==============================================================
// Sliding Window Generator
//==============================================================

void sliding_window_hls(
    const data_t input[MAX_INPUT_SIZE],
    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int input_size,
    int window_size,
    int stride,
    int &num_windows)
{

#pragma HLS INLINE off

    num_windows = 0;

    //----------------------------------------------------------
    // Parameter Validation
    //----------------------------------------------------------

    if (window_size <= 0 ||
        stride <= 0 ||
        input_size < window_size)
    {
        return;
    }

    //----------------------------------------------------------
    // Generate Sliding Windows
    //----------------------------------------------------------

    for (int start = 0;
         start + window_size <= input_size;
         start += stride)
    {

#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_WINDOWS

        for (int i = 0; i < window_size; i++)
        {

#pragma HLS PIPELINE II=1

            windows[num_windows][i] =
                input[start + i];
        }

        num_windows++;
    }
}

//==============================================================
// Apply Window Function
//==============================================================

void apply_window_hls(
    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int num_windows,
    int window_size,
    WindowType window_type)
{

#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=windows complete dim=2

    //----------------------------------------------------------
    // Generate window coefficients
    //----------------------------------------------------------

    data_t coeff[MAX_WINDOW_SIZE];

	#pragma HLS ARRAY_PARTITION variable=coeff complete

    generate_window_coefficients(
        coeff,
        window_size,
        window_type);

    //----------------------------------------------------------
    // Apply coefficients
    //----------------------------------------------------------

    for(int w = 0; w < num_windows; w++)
    {

#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_WINDOWS

        for(int i = 0; i < window_size; i++)
        {

#pragma HLS PIPELINE II=1

            windows[w][i] *= coeff[i];
        }
    }
}


//==============================================================
// Min-Max Normalization
//==============================================================

void min_max_normalize_hls(
    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int num_windows,
    int window_size)
{

#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=windows complete dim=2

	const data_t EPSILON = 0.001f;

    //----------------------------------------------------------
    // Process Each Window
    //----------------------------------------------------------

    for(int w = 0; w < num_windows; w++)
    {

#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_WINDOWS

        //------------------------------------------------------
        // Find Minimum and Maximum
        //------------------------------------------------------

        data_t min_val = windows[w][0];
        data_t max_val = windows[w][0];

        for(int i = 1; i < window_size; i++)
        {

#pragma HLS PIPELINE II=1

            if(windows[w][i] < min_val)
                min_val = windows[w][i];

            if(windows[w][i] > max_val)
                max_val = windows[w][i];
        }

        //------------------------------------------------------
        // Normalize
        //------------------------------------------------------

        data_t range = max_val - min_val;

        if(range < EPSILON)
            range = 1.0f;

        for(int i = 0; i < window_size; i++)
        {

#pragma HLS PIPELINE II=1

            windows[w][i] =
                (windows[w][i] - min_val) /
                range;
        }
    }
}

//==============================================================
// Z-Score Normalization
//==============================================================

void zscore_normalize_hls(
    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int num_windows,
    int window_size)
{

#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=windows complete dim=2

    const data_t EPSILON = 0.001f;

    typedef ap_fixed<32,8> calc_t;

    //----------------------------------------------------------
    // Process Each Window
    //----------------------------------------------------------

    for(int w = 0; w < num_windows; w++)
    {

#pragma HLS LOOP_TRIPCOUNT min=1 max=MAX_WINDOWS

        //------------------------------------------------------
        // Compute Mean
        //------------------------------------------------------

        calc_t mean = 0.0f;

        for(int i = 0; i < window_size; i++)
        {

#pragma HLS PIPELINE II=1

            mean += (calc_t)windows[w][i];
        }

        mean /= window_size;


        //------------------------------------------------------
        // Compute Variance
        //------------------------------------------------------

        calc_t variance = 0.0f;

        for(int i = 0; i < window_size; i++)
        {

#pragma HLS PIPELINE II=1

            calc_t diff = (calc_t)windows[w][i] - mean;

            variance += diff * diff;
        }

        variance /= window_size;


        //------------------------------------------------------
        // Compute Standard Deviation
        //------------------------------------------------------

        data_t stddev = hls::sqrt((data_t)variance);


        if(stddev < EPSILON)
            stddev = 1.0f;


        //------------------------------------------------------
        // Normalize
        //------------------------------------------------------

        for(int i = 0; i < window_size; i++)
        {

#pragma HLS PIPELINE II=1

            calc_t normalized =
                ((calc_t)windows[w][i] - mean) /
                (calc_t)stddev;

            windows[w][i] = (data_t)normalized;
        }
    }
}
//==============================================================
// Normalize Windows
//==============================================================

void normalize_windows_hls(
    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int num_windows,
    int window_size,
    NormalizationType normalization)
{

#pragma HLS INLINE off

    switch(normalization)
    {

    case NormalizationType::NONE:
        break;

    case NormalizationType::MIN_MAX:
        min_max_normalize_hls(
            windows,
            num_windows,
            window_size);
        break;

    case NormalizationType::Z_SCORE:
        zscore_normalize_hls(
            windows,
            num_windows,
            window_size);
        break;
    }
}

//==============================================================
// Top-Level Kernel
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

#pragma HLS INTERFACE m_axi     port=input  offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi     port=output offset=slave bundle=gmem

#pragma HLS INTERFACE s_axilite port=input_size
#pragma HLS INTERFACE s_axilite port=window_size
#pragma HLS INTERFACE s_axilite port=stride
#pragma HLS INTERFACE s_axilite port=window_type
#pragma HLS INTERFACE s_axilite port=normalization
#pragma HLS INTERFACE s_axilite port=num_windows
#pragma HLS INTERFACE s_axilite port=return

    //----------------------------------------------------------
    // Stage 1
    //----------------------------------------------------------

    sliding_window_hls(
        input,
        output,
        input_size,
        window_size,
        stride,
        num_windows);

    //----------------------------------------------------------
    // Stage 2
    //----------------------------------------------------------

    apply_window_hls(
        output,
        num_windows,
        window_size,
        window_type);

    //----------------------------------------------------------
    // Stage 3
    //----------------------------------------------------------

    normalize_windows_hls(
        output,
        num_windows,
        window_size,
        normalization);
}

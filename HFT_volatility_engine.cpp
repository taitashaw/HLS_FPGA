/*** By John Bagshaw ***/

#include "HFT_volatility_engine.hpp"

void fork_input(hls::stream<tick_t>& in,
                hls::stream<tick_t>& preload_in,
                hls::stream<tick_t>& compute_in,
                int window_size) {
    for (int i = 0; i < MAX_WINDOW_SIZE + 128; ++i) {
#pragma HLS PIPELINE II=1
        if (!in.empty()) {
            tick_t tmp = in.read();
            preload_in.write(tmp);
            compute_in.write(tmp);
        }
    }
}

void preload_window(hls::stream<tick_t>& in,
                    hls::stream<price_t>& pre_window_out,
                    hls::stream<tick_t>& last_tick_out,
                    accum_t& sum, accum_t& sq_sum,
                    int window_size) {
    tick_t tick_in;
    sum = 0;
    sq_sum = 0;

    for (int i = 0; i < window_size - 1; ++i) {
#pragma HLS PIPELINE II=1
        tick_in = in.read();
        pre_window_out.write(tick_in.price);
        sum += tick_in.price;
        sq_sum += tick_in.price * tick_in.price;
    }
    last_tick_out.write(tick_in);
}

void compute_variance(hls::stream<tick_t>& in,
                      hls::stream<price_t>& pre_window_in,
                      hls::stream<tick_t>& last_tick_in,
                      hls::stream<signal_t>& out,
                      accum_t sum, accum_t sq_sum,
                      int window_size) {
    price_t window[MAX_WINDOW_SIZE];
#pragma HLS ARRAY_PARTITION variable=window complete dim=1

    for (int i = 0; i < window_size - 1; ++i) {
#pragma HLS PIPELINE II=1
        window[i] = pre_window_in.read();
    }
    tick_t tick_in = last_tick_in.read();

MAIN_LOOP:
    while (!in.empty()) {
#pragma HLS PIPELINE II=1
        tick_in = in.read();
        price_t new_val = tick_in.price;
        price_t old_val = window[0];

        for (int i = 0; i < MAX_WINDOW_SIZE - 1; ++i) {
#pragma HLS UNROLL
            window[i] = window[i + 1];
        }
        window[window_size - 1] = new_val;

        sum += new_val - old_val;
        sq_sum += new_val * new_val - old_val * old_val;

        price_t mean = sum / window_size;
        price_t mean_sq = sq_sum / window_size;
        price_t var = mean_sq - mean * mean;

        signal_t result;
        result.timestamp = tick_in.timestamp;
        result.mean = mean;
        result.variance = var;
        out.write(result);
    }
}

void rolling_variance_stream(hls::stream<tick_t>& in,
                             hls::stream<signal_t>& out,
                             int window_size) {
#pragma HLS DATAFLOW

    hls::stream<tick_t> preload_in("preload_in");
    hls::stream<tick_t> compute_in("compute_in");
    hls::stream<price_t> pre_window_stream("pre_window_stream");
    hls::stream<tick_t> last_tick_stream("last_tick_stream");
#pragma HLS STREAM variable=preload_in depth=128
#pragma HLS STREAM variable=compute_in depth=128
#pragma HLS STREAM variable=pre_window_stream depth=16
#pragma HLS STREAM variable=last_tick_stream depth=2

    accum_t sum = 0;
    accum_t sq_sum = 0;

    fork_input(in, preload_in, compute_in, window_size);
    preload_window(preload_in, pre_window_stream, last_tick_stream, sum, sq_sum, window_size);
    compute_variance(compute_in, pre_window_stream, last_tick_stream, out, sum, sq_sum, window_size);
}

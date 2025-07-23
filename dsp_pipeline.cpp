/*** By John Bagshaw ***/

#include "HLS_dataflow_accel_dsp.hpp"

// Load function (input stream to intermediate stream)
void load_process(hls::stream<data_t>& in, hls::stream<data_t>& out) {
    for (int i = 0; i < 1024; ++i) {
#pragma HLS PIPELINE II=1
        if (!in.empty()) {
            data_t val = in.read();
            out.write(val);
        }
    }
}

// FIR filter process
void fir_filter_process(hls::stream<data_t>& in, hls::stream<data_t>& out) {
    data_t coeffs[3] = {0.25, 0.5, 0.25};
    data_t shift_reg[3] = {0, 0, 0};

    for (int i = 0; i < 1024; ++i) {
#pragma HLS PIPELINE II=1
        if (!in.empty()) {
            shift_reg[2] = shift_reg[1];
            shift_reg[1] = shift_reg[0];
            shift_reg[0] = in.read();

            data_t acc = 0;
            for (int j = 0; j < 3; ++j) {
                acc += coeffs[j] * shift_reg[j];
            }

            out.write(acc);
        }
    }
}

// Gain adjustment process
void gain_adjust_process(hls::stream<data_t>& in, hls::stream<data_t>& out, float gain) {
    for (int i = 0; i < 1024; ++i) {
#pragma HLS PIPELINE II=1
        if (!in.empty()) {
            data_t val = in.read();
            out.write(val * gain);
        }
    }
}

// Store function (intermediate stream to output stream)
void store_process(hls::stream<data_t>& in, hls::stream<data_t>& out) {
    for (int i = 0; i < 1024; ++i) {
#pragma HLS PIPELINE II=1
        if (!in.empty()) {
            out.write(in.read());
        }
    }
}

// Top-level pipeline with dataflow optimization
void dsp_pipeline(hls::stream<data_t>& in_stream,
                  hls::stream<data_t>& out_stream,
                  float gain) {
#pragma HLS DATAFLOW

    hls::stream<data_t> s1("s1"), s2("s2"), s3("s3");
#pragma HLS STREAM variable=s1 depth=8
#pragma HLS STREAM variable=s2 depth=8
#pragma HLS STREAM variable=s3 depth=8

    load_process(in_stream, s1);
    fir_filter_process(s1, s2);
    gain_adjust_process(s2, s3, gain);
    store_process(s3, out_stream);
}

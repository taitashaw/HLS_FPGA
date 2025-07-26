#include "HLS_5G_FIR_chain_accel.hpp"

void load_input(sample_t* in, hls::stream<sample_t>& out_stream, int size) {
    for (int i = 0; i < size; i++) {
    #pragma HLS PIPELINE II=1
        sample_t temp = in[i];
        out_stream.write(temp);
    }
}

void fir_filter(hls::stream<sample_t>& in_stream, hls::stream<sample_t>& out_stream, const coef_t coeffs[TAP_LEN]) {
    sample_t shift_reg[TAP_LEN];
    #pragma HLS ARRAY_PARTITION variable=shift_reg complete

    for (int n = 0; n < N; n++) {
    #pragma HLS PIPELINE II=1
        sample_t in_val = in_stream.read();
        for (int i = TAP_LEN - 1; i > 0; i--) {
            shift_reg[i] = shift_reg[i - 1];
        }
        shift_reg[0] = in_val;

        coef_t acc = 0;
        for (int i = 0; i < TAP_LEN; i++) {
            acc += shift_reg[i] * coeffs[i];
        }
        out_stream.write((sample_t)(acc >> 15));
    }
}

void gain_control(hls::stream<sample_t>& in_stream, hls::stream<sample_t>& out_stream, int gain) {
    for (int i = 0; i < N; i++) {
    #pragma HLS PIPELINE II=1
        sample_t val = in_stream.read();
        out_stream.write(val * gain);
    }
}

void decimator(hls::stream<sample_t>& in_stream, hls::stream<sample_t>& out_stream, int factor) {
    for (int i = 0; i < N; i++) {
    #pragma HLS PIPELINE II=1
        sample_t val = in_stream.read();
        if (i % factor == 0) {
            out_stream.write(val);
        }
    }
}

void store_output(hls::stream<sample_t>& in_stream, sample_t* out, int size) {
    for (int i = 0; i < size; i++) {
    #pragma HLS PIPELINE II=1
        out[i] = in_stream.read();
    }
}

void fir_chain_accel(sample_t* in,
                     sample_t* out,
                     coef_t* coeffs,
                     int gain,
                     int size) {
#pragma HLS INTERFACE m_axi port=in offset=slave bundle=in_gmem depth=N
#pragma HLS INTERFACE m_axi port=out offset=slave bundle=out_gmem depth=N
#pragma HLS INTERFACE m_axi port=coeffs offset=slave bundle=coeffs_gmem depth=TAP_LEN
#pragma HLS INTERFACE s_axilite port=in bundle=control
#pragma HLS INTERFACE s_axilite port=out bundle=control
#pragma HLS INTERFACE s_axilite port=coeffs bundle=control
#pragma HLS INTERFACE s_axilite port=gain bundle=control
#pragma HLS INTERFACE s_axilite port=size bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
#pragma HLS DATAFLOW

    hls::stream<sample_t> s1("s1");
    hls::stream<sample_t> s2("s2");
    hls::stream<sample_t> s3("s3");
    hls::stream<sample_t> s4("s4");
    #pragma HLS STREAM variable=s1 depth=64
    #pragma HLS STREAM variable=s2 depth=64
    #pragma HLS STREAM variable=s3 depth=64
    #pragma HLS STREAM variable=s4 depth=64

    load_input(in, s1, size);
    fir_filter(s1, s2, coeffs);
    gain_control(s2, s3, gain);
    decimator(s3, s4, DECIMATE_FACTOR);
    store_output(s4, out, size / DECIMATE_FACTOR);
}

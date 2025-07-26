/*** By John Bagshaw ***/
#ifndef HLS_5G_FIR_CHAIN_ACCEL_HPP
#define HLS_5G_FIR_CHAIN_ACCEL_HPP

#include "ap_int.h"
#include "hls_stream.h"

#define N 1024
#define TAP_LEN 16
#define GAIN 2
#define DECIMATE_FACTOR 2

typedef ap_int<16> sample_t;
typedef ap_int<32> coef_t;

void load_input(sample_t* in, hls::stream<sample_t>& out_stream, int size);
void fir_filter(hls::stream<sample_t>& in_stream, hls::stream<sample_t>& out_stream, const coef_t coeffs[TAP_LEN]);
void gain_control(hls::stream<sample_t>& in_stream, hls::stream<sample_t>& out_stream, int gain);
void decimator(hls::stream<sample_t>& in_stream, hls::stream<sample_t>& out_stream, int factor);
void store_output(hls::stream<sample_t>& in_stream, sample_t* out, int size);

void fir_chain_accel(sample_t* in,
                     sample_t* out,
                     coef_t* coeffs,
                     int gain,
                     int size);

#endif

// rf_doppler_spoof_engine.hpp
#pragma once
#include <hls_stream.h>
#include <ap_int.h>
#include <ap_fixed.h>
#include <ap_axi_sdata.h>

// ------------------- Build-time constants (global & portable) -------------------
#ifndef FFT_SIZE
#define FFT_SIZE 1024
#endif

#ifndef FFT_OUT_W
#define FFT_OUT_W 16
#endif

#ifndef ALERT_OUT_W
#define ALERT_OUT_W 8
#endif

#ifndef PEAK_THRESHOLD
#define PEAK_THRESHOLD 0.75f
#endif

// Derived (used in pragmas and loops)
#define AXIS_WORDS_PER_FRAME (2*FFT_SIZE)

// ------------------- Public AXI-Stream port types (top-level only) --------------
typedef ap_axiu<FFT_OUT_W, 1, 1, 1> axis_in_t;    // input I/Q samples, 16-bit
typedef ap_axiu<ALERT_OUT_W, 1, 1, 1> axis_out_t; // 8-bit alert byte

// ------------------- Internal fixed-point types ---------------------------------
typedef ap_fixed<16, 4>  fft_data_t;     // I/Q
typedef ap_fixed<32, 8>  magnitude_t;    // |I+jQ|

// ------------------- Top-level prototype ----------------------------------------
void rf_doppler_spoof_engine(hls::stream<axis_in_t>  &adc_stream,
                             hls::stream<axis_out_t> &alert_stream);

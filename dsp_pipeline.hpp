/*** By John Bagshaw ***/

#ifndef HLS_DATAFLOW_ACCEL_DSP_HPP
#define HLS_DATAFLOW_ACCEL_DSP_HPP

#include <hls_stream.h>
#include <ap_int.h>

typedef float data_t;

// Top-level function prototype
void dsp_pipeline(hls::stream<data_t>& in_stream,
                  hls::stream<data_t>& out_stream,
                  float gain);

#endif

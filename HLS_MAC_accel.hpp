#ifndef HLS_MAC_ACCEL_HPP
#define HLS_MAC_ACCEL_HPP

#include <ap_int.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>

typedef ap_int<16> data_t;
typedef ap_int<32> acc_t;
typedef hls::axis<data_t, 0, 0, 0> axis_t;

void load_input(hls::stream<axis_t>& in, data_t& a, data_t& b);
acc_t multiply_accumulate(data_t a, data_t b, data_t coefficient);
void store_result(hls::stream<axis_t>& out, acc_t result);
void mac_top(hls::stream<axis_t>& in, hls::stream<axis_t>& out, data_t coefficient);

#endif

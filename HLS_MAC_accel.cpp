#include "HLS_MAC_accel.hpp"

void load_input(hls::stream<axis_t>& in, data_t& a, data_t& b) {
#pragma HLS INLINE
    axis_t tmp1 = in.read();
    axis_t tmp2 = in.read();
    a = tmp1.data;
    b = tmp2.data;
}

acc_t multiply_accumulate(data_t a, data_t b, data_t coefficient) {
#pragma HLS INLINE OFF
#pragma HLS FUNCTION_INSTANTIATE variable=coefficient
    return (a + b) * coefficient;
}

void store_result(hls::stream<axis_t>& out, acc_t result) {
#pragma HLS INLINE
    axis_t out_val;
    out_val.data = result;
    out_val.last = 1;
    out.write(out_val);
}

void mac_top(hls::stream<axis_t>& in, hls::stream<axis_t>& out, data_t coefficient) {
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out
#pragma HLS INTERFACE s_axilite port=coefficient bundle=CTRL
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    data_t a, b;
    load_input(in, a, b);
    acc_t result = multiply_accumulate(a, b, coefficient);
    store_result(out, result);
}

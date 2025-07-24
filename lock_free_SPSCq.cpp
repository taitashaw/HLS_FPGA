#include "lock_free_SPSCq.hpp"

void spsc_top(hls::stream<data_t> &in,
              hls::stream<data_t> &out,
              volatile data_t buffer_r[QDEPTH],
              volatile queue_state &state) {
#pragma HLS INTERFACE axis        port=in
#pragma HLS INTERFACE axis        port=out
#pragma HLS INTERFACE bram        port=buffer_r
#pragma HLS INTERFACE ap_none     port=state
#pragma HLS INTERFACE ap_ctrl_hs  port=return
#pragma HLS PIPELINE II=1

// ✅ Use modern pragma for memory binding
#pragma HLS bind_storage variable=buffer_r type=ram_1p impl=bram

    // === ENQUEUE ===
    if (!in.empty()) {
        data_t val = in.read();
        ap_uint<10> head_local = state.head;
        ap_uint<10> tail_local = state.tail;
        ap_uint<10> next_tail = (tail_local + 1) % QDEPTH;

        if (next_tail != head_local) {
            buffer_r[tail_local] = val;
            state.tail = next_tail;
        }
    }

    // === DEQUEUE ===
    ap_uint<10> head_local = state.head;
    ap_uint<10> tail_local = state.tail;

    if (head_local != tail_local && !out.full()) {
        data_t val = buffer_r[head_local];
        out.write(val);
        state.head = (head_local + 1) % QDEPTH;
    }
}

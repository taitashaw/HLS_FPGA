#ifndef LOCK_FREE_SPSCQ_HPP
#define LOCK_FREE_SPSCQ_HPP

#include <ap_int.h>
#include <hls_stream.h>

// === FIFO Configuration ===
#define QDEPTH 1024        // Queue depth (must be power of 2 for wraparound)
#define BITS   32          // Bit width of data elements

typedef ap_uint<BITS> data_t;  // Data word type

// === Queue State Structure ===
// Holds head and tail pointers for lock-free SPSC queue
struct queue_state {
    ap_uint<10> head;
    ap_uint<10> tail;
};

// === Top-Level SPSC Queue Function ===
// Note: This function performs both enqueue and dequeue operations
void spsc_top(hls::stream<data_t> &in,
              hls::stream<data_t> &out,
              volatile data_t buffer[QDEPTH],         // BRAM-mapped circular buffer
              volatile queue_state &state);           // Queue metadata (head/tail)

#endif // LOCK_FREE_SPSCQ_HPP

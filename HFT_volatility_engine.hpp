/*** By John Bagshaw ***/

#ifndef HFT_VOLATILITY_ENGINE_HPP
#define HFT_VOLATILITY_ENGINE_HPP

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>

constexpr int MAX_WINDOW_SIZE = 16;
constexpr int SAMPLE_WIDTH = 32;

using price_t = ap_fixed<32, 16>;
using accum_t = ap_fixed<48, 24>;

struct tick_t {
    ap_uint<64> timestamp;
    price_t price;
};

struct signal_t {
    ap_uint<64> timestamp;
    price_t mean;
    price_t variance;
};

void rolling_variance_stream(hls::stream<tick_t>& in,
                             hls::stream<signal_t>& out,
                             int window_size);

#endif // HFT_VOLATILITY_ENGINE_HPP

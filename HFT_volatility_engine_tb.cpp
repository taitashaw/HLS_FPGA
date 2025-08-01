#include "HFT_volatility_engine.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>

#define USE_UNICODE 0

int main() {
    hls::stream<tick_t> in_stream;
    hls::stream<signal_t> out_stream;

    const int N = 128;
    const int WINDOW_SIZE = 3;

    for (int i = 0; i < N; ++i) {
        tick_t tick;
        tick.timestamp = i;
        tick.price = std::sin(i * 0.1f) * 100 + (std::rand() % 10 - 5);
        in_stream.write(tick);
    }

    rolling_variance_stream(in_stream, out_stream, WINDOW_SIZE);

    for (int i = 0; i < N - WINDOW_SIZE + 1; ++i) {
        signal_t sig = out_stream.read();
#if USE_UNICODE
        std::cout << "T=" << sig.timestamp
                  << ", μ=" << sig.mean.to_float()
                  << ", σ²=" << sig.variance.to_float()
                  << std::endl;
#else
        std::cout << "T=" << sig.timestamp
                  << ", mean=" << sig.mean.to_float()
                  << ", var=" << sig.variance.to_float()
                  << std::endl;
#endif
    }

    return 0;
}

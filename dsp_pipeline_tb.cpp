/*** By John Bagshaw ***/

#include <iostream>
#include <cmath>
#include "HLS_dataflow_accel_dsp.hpp"

int main() {
    hls::stream<data_t> in_stream, out_stream;

    // Generate input: sine wave
    for (int i = 0; i < 10; ++i) {
        float val = sinf(2 * M_PI * i / 64);
        in_stream.write(val);
    }

    float gain = 2.5f;
    dsp_pipeline(in_stream, out_stream, gain);

    int count = 0;
    while (!out_stream.empty() && count < 1024) {
        data_t out_val = out_stream.read();
        std::cout << "Output[" << count << "] = " << out_val << std::endl;
        count++;
    }

    return 0;
}


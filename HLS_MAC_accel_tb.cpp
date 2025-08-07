#include "HLS_MAC_accel.hpp"
#include <iostream>

int main() {
    hls::stream<axis_t> in_stream, out_stream;

    axis_t sample1, sample2;
    sample1.data = 10;
    sample1.last = 0;
    sample2.data = 20;
    sample2.last = 0;
    in_stream.write(sample1);
    in_stream.write(sample2);

    data_t coefficient = 3;
    mac_top(in_stream, out_stream, coefficient);

    axis_t result = out_stream.read();
    std::cout << "MAC result: " << result.data << std::endl;

    return 0;
}

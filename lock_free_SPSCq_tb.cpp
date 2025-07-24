#include "lock_free_SPSCq.hpp"
#include <hls_stream.h>
#include <iostream>
#include <iomanip>
#include <chrono>

int main() {
    // === Stream Declarations ===
    hls::stream<data_t> input_stream("input_stream");
    hls::stream<data_t> output_stream("output_stream");

    // === Static Buffer and State ===
    static data_t buffer[QDEPTH];
#pragma HLS bind_storage variable=buffer type=RAM_1P impl=bram
    static queue_state state = {0, 0};

    // === Simulation Configuration ===
    const int num_elements = 10;

    // === Stimulus Phase ===
    for (int cycle = 0; cycle < num_elements * 2; ++cycle) {
        if (cycle < num_elements) {
            input_stream.write(cycle);
        }
        spsc_top(input_stream, output_stream, buffer, state);
    }

    // === Output Verification ===
    std::cout << std::left
              << std::setw(10) << "Index"
              << std::setw(20) << "Timestamp(ns)"
              << std::setw(10) << "Value"
              << std::setw(10) << "Status"
              << "\n---------------------------------------------------------------\n";

    bool all_passed = true;
    int output_count = 0;

    while (!output_stream.empty()) {
        auto now = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

        data_t val = output_stream.read();
        bool correct = (val == output_count);
        if (!correct) all_passed = false;

        std::cout << std::left
                  << std::setw(10) << output_count
                  << std::setw(20) << ns
                  << std::setw(10) << val
                  << std::setw(10) << (correct ? "PASS" : "FAIL")
                  << "\n";

        ++output_count;
    }

    std::cout << "\n=================================================================\n";
    if (all_passed && output_count == num_elements) {
        std::cout << "TEST PASSED: All values correctly passed through the lock-free queue.\n";
    } else {
        std::cout << "TEST FAILED: Output mismatch or missing elements.\n";
    }
    std::cout << "🔢 Total Output Elements: " << output_count << "\n";
    std::cout << "=================================================================\n";

    return (all_passed && output_count == num_elements) ? 0 : 1;
}

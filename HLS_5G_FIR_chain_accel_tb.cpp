/*** By John Bagshaw ***/
#include "HLS_5G_FIR_chain_accel.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    sample_t input[N];
    sample_t output[N / DECIMATE_FACTOR];
    coef_t coeffs[TAP_LEN];

    for (int i = 0; i < N; i++) input[i] = i % 256;
    for (int i = 0; i < TAP_LEN; i++) coeffs[i] = 2048;

    fir_chain_accel(input, output, coeffs, GAIN, N);

    std::cout << "Filtered output (decimated):\n";
    for (int i = 0; i < N / DECIMATE_FACTOR; i++) {
        std::cout << output[i] << std::endl;
    }

    return 0;
}

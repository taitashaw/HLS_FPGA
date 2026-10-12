
# HLS ML Preprocessing Block

Hardware-accelerated machine learning preprocessing kernel implemented using **Vitis HLS**.

This module provides an efficient, hardware-friendly implementation of common preprocessing operations used in time-series, audio, and sensor-based ML pipelines. The design emphasizes fixed-point arithmetic, single-pass streaming architecture, low-resource utilization, and synthesizability on Xilinx FPGAs.

---

## Motivation

Many FPGA-based machine learning systems focus heavily on neural network inference acceleration while preprocessing stages (sliding window extraction, windowing functions, feature normalization) remain CPU-bound. Offloading these preprocessing steps to software creates bandwidth bottlenecks and CPU overhead.

This project implements an end-to-end, single-pass streaming preprocessing pipeline in hardware to enable fully integrated FPGA ML accelerators.

---

## Architecture Overview

The pipeline uses a **Single-Pass Streaming DATAFLOW Architecture**. Intermediate multi-pass external memory round-trips have been eliminated by streaming data concurrently between tasks using `hls::stream<>` FIFOs.


```

```
   [ Input Data (m_axi / Stream) ]
                 │
                 ▼
     ┌───────────────────────┐
     │ Read & Stream Task    │
     └───────────┬───────────┘
                 │ hls::stream
                 ▼
     ┌───────────────────────┐
     │ Window Generator Task │  <-- Buffers only 1 window (MAX_WINDOW_SIZE)
     └───────────┬───────────┘
                 │ hls::stream
                 ▼
     ┌───────────────────────┐
     │ Apply Window Task     │  <-- Dynamic Hamming / Hann / Rectangular
     └───────────┬───────────┘
                 │ hls::stream
                 ▼
     ┌───────────────────────┐
     │ Normalization Task    │  <-- Pipelined Min-Max / Z-Score (II=1)
     └───────────┬───────────┘
                 │
                 ▼
   [ Preprocessed Output (m_axi) ]

```

```

---

## Implemented Features

### 1. Sliding Window Generation
Generates overlapping or contiguous windows from time-series input sequences.
* **Configurable Parameters**: Input size, Window size (up to 64), Stride.
* **Memory Efficiency**: Local storage is restricted to **1 single window buffer** (64 samples) rather than storing the full matrix in external DDR.

### 2. Window Functions
Dynamic coefficient generation supporting window sizes up to `MAX_WINDOW_SIZE = 64`:
* **Rectangular Window**
* **Hamming Window**
* **Hann Window**

### 3. Feature Normalization
* **Min-Max Normalization**: Scales values into the range $[0, 1]$.
  $$\hat{x} = \frac{x - \min(x)}{\max(x) - \min(x)}$$
* **Z-Score Normalization**: Standardizes features to zero mean and unit variance.
  $$\hat{x} = \frac{x - \mu}{\sigma}$$
* **Hardware Optimizations**: 
  * Division operations inside normalization loops are converted to single pre-calculated reciprocal multiplications ($1 / \text{range}$ or $1 / \sigma$), achieving **Initiation Interval (II) = 1**.
  * Replaced square-root divider dependencies with optimized fixed-point arithmetic.

---

## Hardware Implementation Details

### Fixed-Point Precision & Numerics
* **Base Data Type**: `ap_fixed<24,8>` (Provides 8 integer bits and 16 fractional bits).
* **Accumulator Headroom**: Intermediate accumulators in Z-score and normalization tasks are widened to `ap_fixed<40,16>` to provide 16 integer bits of headroom, preventing overflow/saturation during summation across window samples.
* **Precision Preservation**: Standard deviation calculation applies `hls::sqrt()` before casting down to `data_t` resolution.

### HLS Optimizations Applied
* `#pragma HLS DATAFLOW`: Task-level concurrency between memory access, windowing, and normalization.
* `#pragma HLS PIPELINE II=1`: Applied across all inner sample-processing loops.
* **Reciprocal Pre-calculation**: Avoids logic-heavy division hardware inside loop pipelines.
* `#pragma HLS ARRAY_PARTITION`: Applied to local window buffers to enable parallel register-file access.

### Top-Level Interface
```cpp
void ml_preprocessing_hls(
    const data_t input[MAX_INPUT_SIZE],
    data_t output[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int input_size,
    int window_size,
    int stride,
    WindowType window_type,
    NormalizationType normalization,
    int &num_windows
);

```

* **Data Ports**: Configured with `m_axi` (offset=slave, bundle=gmem) with 64-word burst length support.
* **Control Interface**: Configured with `s_axilite` for register control and parameter pass-through.

---

## Synthesis Results

### Configuration & Target Hardware

* **Tool**: Vitis HLS 2023.2
* **Target Device**: Zynq UltraScale+ MPSoC ZCU102 (`xczu9eg-ffvb1156-2-e`)
* **Clock Target**: 10.0 ns (100 MHz) | **Estimated**: ~7.3 ns

### Resource Comparison (Previous vs. Updated DATAFLOW Architecture)

| Resource | Initial Version | Current DATAFLOW Version | Reduction / Gain |
| --- | --- | --- | --- |
| **LUTs** | 178,103 (65.0%) | **22,079 (8.05%)** | **~87.6% Reduction** |
| **FFs** | 147,658 (27.0%) | **18,763 (3.42%)** | **~87.3% Reduction** |
| **BRAMs** | 260 (28.5%) | **12 (1.31%)** | **~95.3% Reduction** |
| **DSPs** | 10 (0.4%) | **34 (1.35%)** | **+24 (Mapped logic to DSPs)** |
| **Memory Passes** | 3 DDR Round-trips | **1 Pass (Streaming)** | **Eliminated DDR bottlenecks** |

---

## Verification & Test Coverage

Self-checking testbench (`tb.cpp`) with non-zero return codes on failure. Verified through **Vitis HLS C Simulation (CSIM)**.

| Test Case | Window Size(s) Tested | Status |
| --- | --- | --- |
| **Sliding Window Generation** | 4, 8, 16 | ✅ PASS |
| **Rectangular Window** | 4, 8, 16 | ✅ PASS |
| **Hamming Window** | 4, 8, 16 | ✅ PASS |
| **Hann Window** | 4, 8, 16 | ✅ PASS |
| **Min-Max Normalization** | 4 | ✅ PASS |
| **Z-Score Normalization** | 4 | ✅ PASS |
| **Full Preprocessing Pipeline** | 4 | ✅ PASS |

---

## Project File Structure

```
├── preprocessing_hls.hpp   # Configuration constants, enums, and function declarations
├── preprocessing_hls.cpp   # DATAFLOW top-level kernel & streaming task implementations
└── tb.cpp                  # Automated self-checking testbench with multi-size coverage

```

---

## License

Distributed under the MIT License.


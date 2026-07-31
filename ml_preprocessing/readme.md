# HLS ML Preprocessing Block

FPGA-oriented machine learning preprocessing kernels implemented using **Vitis HLS**.

This module provides hardware-friendly implementations of common preprocessing operations used in time-series and sensor-based ML pipelines. The design focuses on fixed-point arithmetic, synthesizability, and modular hardware acceleration.

---

## Motivation

Many FPGA-based ML accelerators focus primarily on neural network inference, while preprocessing stages often remain CPU-bound.

This project explores the acceleration of common preprocessing operations on FPGA to enable more complete end-to-end hardware ML pipelines.

---

## Overview

Machine learning models deployed on edge devices often require preprocessing operations such as:

- Sliding window generation
- Window function application
- Feature normalization

Performing these operations in software can introduce additional latency and increase CPU workload.

This module implements a configurable preprocessing pipeline using High-Level Synthesis (HLS):

```
Input Data
    |
    v
Sliding Window Generator
    |
    v
Window Function
(Hamming / Hann / Rectangular)
    |
    v
Normalization
(Min-Max / Z-Score)
    |
    v
Preprocessed Output
```

---

# Implemented Features

## 1. Sliding Window Generation

Generates overlapping windows from input sequences.

### Parameters

- Input size
- Window size
- Stride

### Example

```
Input:
[1 2 3 4 5 6 7 8]

Window size = 4
Stride = 2

Output:

[1 2 3 4]
[3 4 5 6]
[5 6 7 8]
```

---

## 2. Window Functions

Supported window functions:

- Rectangular Window
- Hamming Window
- Hann Window

Window coefficients are precomputed and stored as constants to avoid runtime trigonometric computations.

---

## 3. Normalization

Implemented normalization methods:

### Min-Max Normalization

Transforms values into the range:

```
[0,1]
```

Formula:

```
x' = (x - min(x)) / (max(x) - min(x))
```

---

### Z-Score Normalization

Standardizes input features:

```
x' = (x - mean) / stddev
```

---

# Hardware Implementation Details

## Arithmetic

The design uses fixed-point arithmetic:

```cpp
ap_fixed<24,8>
```

The precision was selected to provide a balance between:

- Numerical accuracy
- FPGA resource utilization
- Hardware efficiency

---

## HLS Optimizations

The current implementation includes:

- Loop pipelining
- Array partitioning
- Precomputed coefficients
- Modular HLS functions

Example:

```cpp
#pragma HLS PIPELINE II=1
```

---

# Interface

Top-level HLS kernel:

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

The kernel supports FPGA integration using:

- AXI4 Master interface for data movement
- AXI4-Lite interface for control parameters

---

# Verification

The module was verified using **Vitis HLS C Simulation**.

## Test Coverage

| Module                       | Status |
|------------------------------|--------|
| Sliding Window Generation    | ✅ PASS |
| Hamming Window               | ✅ PASS |
| Min-Max Normalization        | ✅ PASS |
| Z-Score Normalization        | ✅ PASS |
| Complete Processing Pipeline | ✅ PASS |



---

# Synthesis Results

Target: Zynq Ultrascale+MPSoC zcu102

```
Vitis HLS 2023.2
Clock Target: 10 ns (100 MHz)
```

## Resource Utilization

| Resource | Usage |
|----------|------:|
| BRAM     | 260 |
| DSP      | 10 |
| FF       | 147658 |
| LUT      | 178103 |
| URAM     | 0 |

---

# Current Limitations

The current version prioritizes functional correctness and HLS compatibility.

Future improvements include:

- Optimize division operations using reciprocal approximation
- Replace generic square-root implementation with FPGA-efficient approximation
- Explore AXI-Stream based dataflow architecture
- Improve memory architecture for higher throughput
- Evaluate reduced fixed-point precision for resource optimization

---

# Future Work

Planned extensions:

- Additional preprocessing kernels
- Streaming preprocessing pipeline
- Resource and latency optimized implementations
- Integration with FPGA ML inference accelerators

---

# License

(Add project license information)

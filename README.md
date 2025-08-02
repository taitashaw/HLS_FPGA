# HLS_FPGA
🚀 Real-Time High-Level Synthesis (HLS) Projects for UltraScale+ FPGAs. Accelerated design pipelines, dataflow architectures, and low-latency compute cores — powered by Vitis HLS, AXI4-Stream, and signal processing for 5G, SDR, and HFT applications. 🔧🔬💡
# 🔥 HLS_FPGA: High-Level Synthesis Design Vault

Welcome to the **HLS_FPGA** repository — a cutting-edge collection of **real-world, production-grade** FPGA accelerators built with **Vitis HLS**, tailored for:

- 🛰️ Software-Defined Radio (SDR)
- 📡 5G / O-RAN Signal Processing
- 📈 High-Frequency Trading (HFT)
- 🧠 AI/ML Edge Inference
- 🔐 Embedded Security Accelerators

## 🚀 Key Features
- Fully pipelined, low-latency compute chains (filter → gain → envelope)
- AXI4-Stream + AXI-MM optimized for UltraScale+ SoCs
- Complete Vivado-ready project structure
- Benchmark reports: latency, throughput, utilization
- 🧪 Co-Sim & Waveform validation included

## 🧠 Concepts Covered
- `#pragma HLS DATAFLOW`, `PIPELINE`, `UNROLL`
- `hls::stream`, `ap_fixed`, `hls::vector`
- Loop optimization, interface pragmas, and dataflow chaining
- AXI4-Lite control interface + AXI-MM streaming buffer logic

## 💡 Use Cases
- Real-time DSP on FPGAs
- Hardware-accelerated quant finance
- Latency-critical radio and vision systems

## 🤝 Contribute
Want to optimize a pipeline, add a new design, or share your performance results? Fork this repo and open a PR. Let’s redefine **hardware acceleration** together.

---

### 📣 Let's make FPGAs mainstream for developers and innovators everywhere. Star ⭐ the repo, share with your network, and ignite the hardware revolution!

# 🚨 BREAKING: FPGA-Accelerated DSP Pipelines ⚡📈

Just completed — a fully pipelined, real-time DSP accelerator optimized using Vitis HLS for High-Frequency Trading, signal intelligence, and low-latency inferencing.

🧠 **Project: `dsp_pipeline` — HLS + AXI4-Stream + Dataflow** 🔥

---

## 🚀 Why This Is Mind-Blowing

✅ **Dataflow Rocket:** Every block—`load_process`, `fir_filter`, `gain_adjust`, `store_process`—runs *truly in parallel*. No bubbles. Ultrafast throughput.  
✅ **Timing Dominated:** Required: 10ns. Achieved: **4.641ns**. That’s 216+ MHz Fmax *without even trying*.  
✅ **DSP-Efficient:** Just **18 DSPs**, 0 BRAMs, 0 URAMs. *UltraLow-resource. UltraFast.*  
✅ **Latency Optimized:** Full processing latency = **1,052 cycles** (under 10μs @ 100MHz)  
✅ **Waveform Proven:** Cosim ✅. RTL trace ✅. FIFO depth ✅. Parallel timeline ✅. *Every bit validated.*

---

## 💥 Why It Matters for HFT

> When **nanoseconds = millions**, a pipelined FPGA-based DSP engine like this can preprocess, filter, amplify, and route market signals *before a single CPU interrupt.*  
No latency tax. No wasted cycles.

---

## 🔁 Use Cases

- Market signal denoising before tick injection  
- Real-time RF front-ends (e.g., GNSS, O-RAN, EW)  
- Quant pipelines for proprietary strategy compute  
- On-FPGA ML feature pre-processing (waveforms → vectors)

---

## 📸 Visual Validation Gallery

| Report | Preview |
|--------|---------|
| 🔧 **Post-Implementation Resource Usage** | ![Synthesis Table](https://github.com/user-attachments/assets/212fb321-d8fe-416e-9339-ad8cb8ef883d) |
| ⏱️ **Co-Simulation Timeline Trace** | ![Timeline Trace](https://github.com/user-attachments/assets/8491d37f-bff0-4b1f-9551-9d0536f58929) |
| 🔁 **Waveform Trace** | ![Waveform 1](https://github.com/user-attachments/assets/e5218fd7-144e-4f04-9f85-4ed7cafcbcbb) ![Waveform 2](https://github.com/user-attachments/assets/945a77bc-58b6-4c21-9929-91fbc9c7f156)|
| 🧠 **Code Analyzer** | ![Code Analyzer](https://github.com/user-attachments/assets/000f4d82-1041-4a16-995a-62b8cbbaffb6) |
| 🧮 **Schedule Viewer** | ![Schedule Viewer](https://github.com/user-attachments/assets/121f71f1-5c43-41b2-8b6a-640aa080e55c) |
| 📊 **Synthesis Report** | ![Synthesis Report](https://github.com/user-attachments/assets/f90eb376-ad13-4861-8a48-ab5f095f2e95) |
| 📟 **C Simulation Output** | ![C Sim Output](https://github.com/user-attachments/assets/57a67bc5-8488-4f5a-8fd7-041b5db09778) |

 # 🚀 Lock-Free FPGA Queue for High-Frequency Trading (HFT)

This is not your average FIFO. This is a fully pipelined, BRAM-backed, single-producer single-consumer (SPSC) queue designed to obliterate lock-based latency in high-frequency trading systems.

---

## 🔥 Why It Matters

When **nanoseconds = millions**, software locks just don’t cut it. This lock-free SPSC queue:

- ❌ No locks
- ❌ No FIFOs
- ❌ No DSPs
- ✅ Fully pipelined
- ✅ 100% deterministic timing
- ✅ BRAM circular buffer
- ✅ Synthesized with II=1, latency = 5 cycles
- ✅ CoSim, CSim, and Waveform ✅

---

## 💡 Built For:

- FIX/ITCH packet pipelines
- Market snapshot replay
- FPGA risk-aware strategy engines
- Predictable FIFO-less queuing across accelerators

---

## 📊 Performance Snapshot

| Metric | Value |
|--------|-------|
| CoSim | ✅ Passed |
| II | 1 |
| Latency | 5 cycles |
| FFs | 38 |
| LUTs | 175 |
| BRAM | 1 |
| URAM | 0 |
| DSPs | 0 |

---

## 🧪 Visual Proof

| Preview | Description |
|--------|-------------|
| ![](https://github.com/user-attachments/assets/3fae93d7-d69b-41c9-adfd-ad5aadf80cc4) | AXI4 + BRAM Block Design |
| ![](https://github.com/user-attachments/assets/4553a229-ee3a-477c-a52b-b92137bf0557) | RTL Cosim Waveform |
| ![](https://github.com/user-attachments/assets/b868f047-17e3-4b0c-9421-e4d40fdde148) | RTL Cosim |
| ![](https://github.com/user-attachments/assets/b0b61c40-aa94-441a-ad46-dd3633b02258) | Vivado Synthesis |
| ![](https://github.com/user-attachments/assets/3da5a1ef-96d8-4d20-9532-2de46badb9a9) | Real Trade Loop Output |

---

## 🙌 Contribute

Ideas? Want to scale this to MPMC or adaptive queues? PRs welcome.

---

## 🙌 Contribute

Pull requests, forks, and optimizations welcome. 

# 🚀 HLS-based 5G FIR Chain Accelerator

> **📈 Ultra-efficient, pipelined, and zero-stall FIR+Gain+Decimation DSP Engine built with Vitis HLS**

## 🔥 Project Summary

Imagine streaming 1024 samples through a **FIR → Gain → Decimation** pipeline in **~5µs** on FPGA — no kernel, no interrupts, no stalls.  
We built it. It’s fast, deterministic, and verified.

This HLS project demonstrates a fully pipelined digital signal processing accelerator optimized for **5G PHY**, **radar DSP**, and **ultra-low-latency financial compute (HFT)**.

---

## ⛓️ Processing Pipeline

load_input()
↓
fir_filter()
↓
gain_control()
↓
decimator()
↓
store_output()

✅ Built with `#pragma HLS DATAFLOW` and `hls::stream` for full task-level parallelism  
✅ AXI4-Stream interfaces, scalable, synthesizable  
✅ UltraScale+ friendly (ZCU104/ZCU9EG)  
✅ Bit-accurate waveform and timing validated

---

## 💡 Technical Highlights

| Feature                        | Status |
|-------------------------------|--------|
| ⏱ FIR Latency                 | ~1,052 cycles |
| 🚀 Max Frequency (Fmax)       | > 200 MHz |
| 🧠 DSP Slice Usage            | 18 DSPs only |
| 📦 BRAM / URAM Usage          | Zero |
| 📉 Decimation Factor          | 2× |
| ✅ Timing Closure             | ✔️ Clean |
| 🔬 Waveform Match             | ✔️ Co-sim + RTL |

---

## 📸 Waveform & Benchmark Snapshots

| Co-Simulation | Cycle-Accurate | Synthesis Report |
|---------------|----------------|------------------|
| ![WF](https://github.com/user-attachments/assets/161907aa-4e97-4306-b0b6-23d2d3338f96) | ![CA](https://github.com/user-attachments/assets/1e25ace1-4809-416b-8728-9f07e0a3cd28) | ![SYN](https://github.com/user-attachments/assets/dbdf69d1-aa19-4b13-915a-3848912aab23) |

| Latency Timeline | Vivado Block Design | Vitis HLS Design Implementation |
|------------------|---------------------|---------------------|
| ![TT](https://github.com/user-attachments/assets/61668b91-07ac-4db7-8913-cc55b8a24a57) | ![BD](https://github.com/user-attachments/assets/b9d2e924-e158-48d7-84bc-fe5cd52f3c77) | ![DI](https://github.com/user-attachments/assets/0547ed4c-6fa4-4560-8075-2c6e95188442) |

| Benchmark |
|---------------|
| ![Benchmark](https://github.com/user-attachments/assets/6b355613-5841-40cf-9656-778356b3be9d) |

---

### 🔍 Quick Analysis

| Metric                          | Value                          |
|--------------------------------|---------------------------------|
| 🔄 Full Pipeline Latency       | 1,052 cycles                    |
| 🧠 DSP Usage                   | 17 DSPs                         |
| 🎯 Fmax Achieved (Post-Impl)  | **> 214 MHz** (4.671ns period)  |
| ⚙️ Timing Closure              | ✅ Achieved                     |
| 🧩 Slices / LUT / FF           | 0 / 3431 / 4256                 |
---

## 🎯 Use Cases

- 📡 **5G Uplink/Downlink PHY Chains**
- 🛰️ **Radar Signal Envelope Filtering**
- 💸 **HFT Market Signal Preprocessing**
- 🤖 **Edge AI Signal Cleaning (Pre-ML)**

---

## 🧪 How to Run Locally
## 1. Open in Vitis HLS 2023.2 or later
## 2. Import source files
## 3. Run C Simulation
## 4. Run C/RTL Co-Simulation
## 5. Check reports for Fmax and utilization

# 🚀 HFT_Volatility_Engine

**A Real-Time Rolling Volatility Engine Designed for High-Frequency Trading — in Hardware.**

This project computes rolling variance of tick price streams using a fully pipelined architecture implemented in **Vitis HLS** with **AXI4-Stream** interfaces.
It mimics real-world financial models used in **HFT**, **portfolio risk estimation**, and **embedded analytics engines** — but executes **entirely in silicon**.

---

## 💡 Key Features

- ✅ Zero software latency — pure hardware acceleration  
- ✅ Fully pipelined (II = 1) for deterministic throughput  
- ✅ Real-time streaming via `hls::stream<tick_t>`  
- ✅ Fixed-point arithmetic with `ap_fixed`  
- ✅ Modular stages: `fork_input`, `preload_window`, `compute_variance`  
- ✅ Validated via waveform, CSim, CoSim, synthesis, and timing  

---

## 📈 Rolling Variance Formula

```
σ² = (Σxᵢ²)/W − (Σxᵢ/W)²
```

Where:
- **W** = rolling window size  
- **xᵢ** = tick price at time `i`

---

## ⚙️ How It Works

1. `fork_input()` splits tick input into two streaming branches  
2. `preload_window()` initializes window sums for mean and variance  
3. `compute_variance()` maintains a sliding window using shift registers, computing:

All modules are connected using `hls::stream` and operate concurrently under `#pragma HLS DATAFLOW`.

---

## 📊 Performance Summary

| Metric       | Value         |
|--------------|---------------|
| **Latency**  | 6.079 ns      |
| **Fmax**     | 151.72 MHz    |
| **II**       | 1             |
| **DSPs**     | 16            |
| **LUTs**     | 8,587         |
| **FFs**      | 9,693         |
| **BRAMs**    | 6             |

---

## 📸 Waveform & Benchmark Snapshots

| Co-Simulation Output | Synthesis Utilization | Latency Timeline | Testbench Waveform 1 | Testbench Waveform 2 |
|----------------------|------------------------|------------------|--------------------|------------------------|
| ![RTL Cosimulation](https://github.com/user-attachments/assets/cfbef389-aa19-4c9c-bd20-e9fdd2d9a819) | ![Synthesis](https://github.com/user-attachments/assets/98e4e073-fcc4-4e99-b521-f3fc3d14d2dd) | ![Timeline](https://github.com/user-attachments/assets/2fd51463-bd83-40da-ad05-c449d8bd0305) |  ![Waveform1](https://github.com/user-attachments/assets/c1d9098d-15df-42ff-b10a-bfe57df7ffdf) | ![Waveform2](https://github.com/user-attachments/assets/374685f0-4c03-4e3e-9f48-c14965be6cd2)

## 💬 Real-World Use Cases

- ⚡ FPGA-native volatility filters in high-frequency trading
- 📉 DeFi and crypto risk analytics with ultra-low latency
- 🧠 AI models fed by hardware-accelerated financial stats
- 🚀 Edge inference on risk/volatility for embedded finance

---

## 🤝 Contributing

Pull requests are welcome.  
Feel free to fork and enhance — new features, new financial indicators, AXI-MM integration, or CI automation.

---

## 👤 Author

**John Bagshaw**  
Award-winning researcher and HLS/FPGA/C++ Engineer  
🔗 [LinkedIn](https://linkedin.com/in/jotshaw)

> DM me if you're hiring, collaborating, or building FPGA-based quant infrastructure.

---

## ⭐ Support

If this project inspires you:
- ⭐ Star the repo  
- 🔁 Share with fellow quant or hardware devs  
- 💬 Comment your ideas or feature requests

---

**Let’s build finance at the speed of light. 🚀**  
**Silicon-native volatility is here.**

# 🚀 MAC_Accelerator_HLS

**Real-Time Multiply-Accumulate Hardware Accelerator** built using **Vitis HLS** for DSP/AI workloads.  
🧠 Pipelined. 💡 Coefficient-Instantiated. ⚡ Ultra-low-latency.  
🎯 Ideal for real-time 5G PHY, radar pipelines, CNN inference, and embedded vision systems.

---

## 🔧 Architecture Overview

AXI4-Stream Input
↓
load_input()
↓
multiply_accumulate()
↓
store_result()
↓
AXI4-Stream Output

---

## 📦 Features

- ✅ **Fully Pipelined** Top-Level MAC
- ✅ **Function Instantiation** for coefficient-based specialization
- ✅ **AXI4-Stream I/O + AXI-Lite Control** (Vivado/Vitis Ready)
- ✅ **II = 1** (Initiation Interval of 1) for Real-Time Performance
- ✅ **Waveform, Cosim, Synthesis Snapshots Included**

---

## 🧠 Core Functions

// Reads two operands from AXI4-Stream
- void load_input(hls::stream<axis_t>& in, data_t& a, data_t& b);

// Computes (a + b) * coefficient
- acc_t multiply_accumulate(data_t a, data_t b, data_t coefficient);

// Streams result to AXI4-Stream
- void store_result(hls::stream<axis_t>& out, acc_t result);

- 🔁 mac_top(...) wires it all into a fully pipelined, real-time MAC unit.

## 🧠 Core Functions
---

## 🧪 Testbench Output

MAC result: 90

---

### 📸 Visual Highlights

#### 3-Panel View

| Co-Simulation Waveform                          | Throughput Timeline                           | Synthesis Report                             |
|--------------------------------------------------|------------------------------------------------|----------------------------------------------|
| ![Waveform](https://github.com/user-attachments/assets/34a0d6ed-0e9f-442b-8508-81bacd5990f8) | ![Timeline](https://github.com/user-attachments/assets/f1ee2394-b121-44fe-81ef-3f28f228a53b) | ![Synthesis](https://github.com/user-attachments/assets/b89d929c-b167-4664-a01c-0d7df6544997) |

#### 2-Panel View

| Implementation Block Diagram                    | Control Analysis                               |
|--------------------------------------------------|------------------------------------------------|
| ![Block Diagram](https://github.com/user-attachments/assets/1c275031-52a2-4d25-a277-1f16eb2ae7ba) | ![Control Analysis](https://github.com/user-attachments/assets/071552c2-9e3e-4e23-92ad-f8c60fd7ab8b) |

---

### 📂 Files Included

| File Name               | Description                                   |
|-------------------------|-----------------------------------------------|
| `HLS_MAC_accel.hpp`     | Top-level function and type declarations      |
| `HLS_MAC_accel.cpp`     | Pipeline logic with function directives       |
| `HLS_MAC_accel_tb.cpp`  | AXI stream testbench simulation               |

---

### 🔗 Interface Summary

| Port Name                | Description                                 |
|--------------------------|---------------------------------------------|
| `input_stream_operands` | AXI4-Stream input (two operands)            |
| `output_stream_result`  | AXI4-Stream output (MAC result)             |
| `coefficient`           | AXI4-Lite control register for MAC weight   |
| `ctrl_axi_lite`         | AXI4-Lite interface                         |
| `mac_clk`               | Clock signal                                |
| `mac_reset_n`           | Active-low reset                            |
| `mac_done_interrupt`    | Signals when result is valid                |

---

### 💡 Real-World Applications

- 📶 **5G Baseband DSP**
- 🧠 **Edge AI Inference Engines**
- 🛰️ **Satellite Imaging & Radar**
- 💻 **Embedded Vision SoCs**
- 📊 **High-Frequency Financial Analytics**

---

### 🤝 Contributing

Want to port this to Zynq, add burst DMA, or wrap it in a full CNN accelerator?  
**Fork, enhance, and tag me!**

---

### 🔥 Let’s Go Viral Together

If you're building ultra-low-latency pipelines, this project is your launchpad.  
⭐ **Star this repo** and share your MAC benchmarks.

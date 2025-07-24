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

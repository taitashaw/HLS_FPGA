// tb_rf_doppler_spoof_engine.cpp
// Industry-style, self-checking testbench for rf_doppler_spoof_engine
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstring>
#include "rf_doppler_spoof_engine.hpp"

// Match DUT constants
static const float  THRESHOLD = PEAK_THRESHOLD;
static const int    EDGE_W    = 5;

// Drive all sidebands for clean waveforms
static inline axis_in_t make_axis_in(ap_uint<FFT_OUT_W> data, bool last) {
    axis_in_t a = {};
    a.data = data;
    a.keep = 0x1;   // one byte “kept” (matches 8b granularity)
    a.strb = 0x1;
    a.last = last ? 1 : 0;
    a.user = 0; a.id = 0; a.dest = 0;
    return a;
}

// Interleaved I/Q: R0,Q0,R1,Q1,... with TLAST on last Q
static inline void push_frame_from_vectors(hls::stream<axis_in_t> &s,
                                           const std::vector<float> &re,
                                           const std::vector<float> &im)
{
    for (int i = 0; i < FFT_SIZE; ++i) {
        // Keep I samples “visible” in the waveform: 0x0007 for strong bins, 0x0001 for background
        ap_int<FFT_OUT_W> rv = (ap_int<FFT_OUT_W>)std::lrintf(re[i]);  // e.g., 7 or 1
        ap_int<FFT_OUT_W> qv = (ap_int<FFT_OUT_W>)std::lrintf(im[i]);  // 0 most of the time

        axis_in_t I = make_axis_in((ap_uint<FFT_OUT_W>)rv, false);
        axis_in_t Q = make_axis_in((ap_uint<FFT_OUT_W>)qv, (i == FFT_SIZE-1));

        s.write(I);
        s.write(Q);
    }
}

static inline bool pop_alert_byte(hls::stream<axis_out_t> &out, uint8_t &val, bool &last)
{
    if (out.empty()) return false;
    axis_out_t w = out.read();
    val  = (uint8_t)w.data.to_uint();
    last = (w.last != 0);
    return true;
}

// Golden check
static bool golden_spoof(const std::vector<float> &re,
                         const std::vector<float> &im,
                         int &peak_bin, float &peak_mag)
{
    peak_mag = -1.0f; peak_bin = 0;
    for (int i = 0; i < FFT_SIZE; ++i) {
        float m = std::sqrt(re[i]*re[i] + im[i]*im[i]);
        if (m > peak_mag) { peak_mag = m; peak_bin = i; }
    }
    return (peak_mag > THRESHOLD) && (peak_bin < EDGE_W || peak_bin > (FFT_SIZE-1-EDGE_W));
}

// Vectors: choose “visible” integer data so the waveform shows non-zero TDATA
static void gen_edge_spoof(std::vector<float> &re, std::vector<float> &im)
{
    re.assign(FFT_SIZE, 1.0f);   // background -> 0x0001
    im.assign(FFT_SIZE, 0.0f);
    re[0] = 7.0f;                // strong edge -> 0x0007
}

static void gen_center_clean(std::vector<float> &re, std::vector<float> &im)
{
    re.assign(FFT_SIZE, 1.0f);
    im.assign(FFT_SIZE, 0.0f);
    re[FFT_SIZE/2] = 7.0f;       // strong center -> clean by edge rule
}

static void gen_edge_weak(std::vector<float> &re, std::vector<float> &im)
{
    re.assign(FFT_SIZE, 1.0f);
    im.assign(FFT_SIZE, 0.0f);
    re[1] = 1.0f;                // near edge but not strong (same as background)
}

static void print_case_header(const char *name)
{
    std::cout << "\n=== " << name << " ===\n";
    std::cout << "FFT_SIZE=" << FFT_SIZE
              << " THRESHOLD=" << THRESHOLD
              << " EDGE_W=" << EDGE_W << "\n";
}

static void print_result(const char *name,
                         bool dut_spoof, uint8_t dut_byte, bool dut_last,
                         bool ref_spoof, int peak_bin, float peak_mag)
{
    using std::cout; using std::hex; using std::dec; using std::setfill; using std::setw; using std::fixed; using std::setprecision;

    cout << "DUT  : alert_byte=0x" << hex << setw(2) << setfill('0') << (unsigned)dut_byte
         << dec << "  TLAST=" << (int)dut_last
         << "  SPOOF=" << (dut_spoof ? "YES" : "NO") << "\n";

    cout << "GOLD : peak_bin=" << peak_bin
         << "  peak_mag=" << fixed << setprecision(3) << peak_mag
         << "  SPOOF=" << (ref_spoof ? "YES" : "NO") << "\n";

    bool pass = (dut_spoof == ref_spoof);
    cout << "[ " << name << " ] => " << (pass ? "PASS" : "FAIL") << "\n";
}

int main()
{
    hls::stream<axis_in_t>  in;
    hls::stream<axis_out_t> out;

    int peak_bin; float peak_mag;
    bool ref_spoof, dut_spoof;
    uint8_t dut_byte; bool dut_last;

    // CASE 1: Edge spoof (expect YES)
    {
        std::vector<float> re, im; gen_edge_spoof(re, im);
        print_case_header("CASE 1: Edge spoof");
        ref_spoof = golden_spoof(re, im, peak_bin, peak_mag);
        push_frame_from_vectors(in, re, im);
        rf_doppler_spoof_engine(in, out);
        if (!pop_alert_byte(out, dut_byte, dut_last)) { std::cout << "No alert produced\n"; return 1; }
        dut_spoof = (dut_byte & 0x1);
        print_result("CASE 1", dut_spoof, dut_byte, dut_last, ref_spoof, peak_bin, peak_mag);
    }

    // CASE 2: Strong center (expect NO)
    {
        std::vector<float> re, im; gen_center_clean(re, im);
        print_case_header("CASE 2: Center strong (clean)");
        ref_spoof = golden_spoof(re, im, peak_bin, peak_mag);
        push_frame_from_vectors(in, re, im);
        rf_doppler_spoof_engine(in, out);
        if (!pop_alert_byte(out, dut_byte, dut_last)) { std::cout << "No alert produced\n"; return 1; }
        dut_spoof = (dut_byte & 0x1);
        print_result("CASE 2", dut_spoof, dut_byte, dut_last, ref_spoof, peak_bin, peak_mag);
    }

    // CASE 3: Edge weak (expect NO)
    {
        std::vector<float> re, im; gen_edge_weak(re, im);
        print_case_header("CASE 3: Edge weak (below threshold)");
        ref_spoof = golden_spoof(re, im, peak_bin, peak_mag);
        push_frame_from_vectors(in, re, im);
        rf_doppler_spoof_engine(in, out);
        if (!pop_alert_byte(out, dut_byte, dut_last)) { std::cout << "No alert produced\n"; return 1; }
        dut_spoof = (dut_byte & 0x1);
        print_result("CASE 3", dut_spoof, dut_byte, dut_last, ref_spoof, peak_bin, peak_mag);
    }

    std::cout << "\nAll tests completed.\n";
    return 0;
}

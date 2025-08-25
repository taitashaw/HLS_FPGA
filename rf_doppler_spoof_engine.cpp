// GNSS-Aware Doppler + Spoof Detection Engine
// Vitis HLS 2025.1+ (Vivado IP flow)
/*** By John Bagshaw ***/

#include "rf_doppler_spoof_engine.hpp"
#include <hls_math.h>

// ---------------- [Module 1] adc_demux: de-multiplexes interleaved ADC I/Q—one AXIS read per cycle; even→real, odd→imag; II=1 over AXIS_WORDS_PER_FRAME (backpressure-safe). ----------------
static void adc_demux(hls::stream<axis_in_t>  &adc_stream,
                      hls::stream<fft_data_t> &real_stream,
                      hls::stream<fft_data_t> &imag_stream)
{
#pragma HLS INLINE off
demux_loop:
  for (int n = 0; n < AXIS_WORDS_PER_FRAME; ++n) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=AXIS_WORDS_PER_FRAME max=AXIS_WORDS_PER_FRAME
    axis_in_t x = adc_stream.read();
    fft_data_t v = fft_data_t(x.data);
    if ((n & 1) == 0) real_stream.write(v);
    else              imag_stream.write(v);
  }
}

// ---------------- [Module 2] magnitude_estimator: computes |I+jQ|—consumes FFT_SIZE aligned I/Q stream pairs and emits one magnitude per cycle (II=1) via HLS sqrt (CORDIC); backpressure-safe. ----------------
static void magnitude_estimator(hls::stream<fft_data_t>  &real_stream,
                                hls::stream<fft_data_t>  &imag_stream,
                                hls::stream<magnitude_t> &mag_stream)
{
#pragma HLS INLINE off
mag_loop:
  for (int i = 0; i < FFT_SIZE; ++i) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=FFT_SIZE max=FFT_SIZE
    fft_data_t r = real_stream.read();
    fft_data_t q = imag_stream.read();
    magnitude_t m = hls::sqrt(magnitude_t(r)*magnitude_t(r) + magnitude_t(q)*magnitude_t(q));
    mag_stream.write(m);
  }
}

// ---------------- [Module 3] peak_detector: scans FFT_SIZE magnitudes to find max value and its bin index; II=1 inner pass, single write-out of {bin,max}; preserves stream order. ----------------
static void peak_detector(hls::stream<magnitude_t> &mag_stream,
                          hls::stream<ap_uint<10> > &peak_bin,
                          hls::stream<magnitude_t>  &peak_mag)
{
#pragma HLS INLINE off
  magnitude_t maxv = 0;
  ap_uint<10> maxi = 0;
pk_loop:
  for (int i = 0; i < FFT_SIZE; ++i) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=FFT_SIZE max=FFT_SIZE
    magnitude_t v = mag_stream.read();
    if (v > maxv) { maxv = v; maxi = (ap_uint<10>)i; }
  }
  peak_bin.write(maxi);
  peak_mag.write(maxv);
}

// ---------------- [Module 4] spoof_detector: applies near-edge & strong-peak rule to emit 8-bit alert_code (bit0=spoof); one-cycle processing of {peak_bin,peak_mag}. ----------------
static void spoof_detector(hls::stream<ap_uint<10> > &peak_bin,
                           hls::stream<magnitude_t>  &peak_mag,
                           hls::stream<ap_uint<8> >  &alert_code)
{
#pragma HLS INLINE off
#pragma HLS PIPELINE II=1
  ap_uint<8> code = 0;
  ap_uint<10> bin = peak_bin.read();
  magnitude_t mag = peak_mag.read();

  // Edge + strong threshold rule (bit0 marks spoof)
  const bool near_edge = (bin < 5) || (bin > (FFT_SIZE - 6));
  const bool strong    = (mag > magnitude_t(PEAK_THRESHOLD));
  if (near_edge && strong) code[0] = 1;

  alert_code.write(code);
}

// ---------------- [Module 5] alert_packer: packs alert_code into a single AXI4-Stream beat (.keep/.strb=0x1, .last=1) for host consumption. ----------------
static void alert_packer(hls::stream<ap_uint<8> > &alert_code,
                         hls::stream<axis_out_t>  &alert_stream)
{
#pragma HLS INLINE off
#pragma HLS PIPELINE II=1
  ap_uint<8> code = alert_code.read();

  axis_out_t w;
  w.data = code;
  w.keep = 0x1;  // one byte valid
  w.strb = 0x1;
  w.last = 1;    // single-beat frame
  w.user = 0;
  w.id   = 0;
  w.dest = 0;

  alert_stream.write(w);
}

// ---------------- [Top Module] rf_doppler_spoof_engine: canonical DATAFLOW composition—registers AXIS ports, uses ap_ctrl_chain, declares/decouples streams, and wires Modules 1→5 end-to-end. ----------------
void rf_doppler_spoof_engine(hls::stream<axis_in_t>  &adc_stream,
                             hls::stream<axis_out_t> &alert_stream)
{
    // Register both input and output AXIS ports so signals come out of reset as 0
#pragma HLS INTERFACE axis          port=adc_stream    register_mode=both register
#pragma HLS INTERFACE axis          port=alert_stream  register_mode=both register

    // Use a control protocol with start/ready/done to avoid auto-rewind warnings
#pragma HLS INTERFACE ap_ctrl_chain port=return
#pragma HLS DATAFLOW

    // stream declarations and function calls...
    hls::stream<fft_data_t>   real_stream("real_stream");
    hls::stream<fft_data_t>   imag_stream("imag_stream");
    hls::stream<magnitude_t>  mag_stream ("mag_stream");
    hls::stream<ap_uint<10> > pk_bin     ("pk_bin");
    hls::stream<magnitude_t>  pk_mag     ("pk_mag");
    hls::stream<ap_uint<8> >  alert_code ("alert_code");

#pragma HLS STREAM variable=real_stream depth=32
#pragma HLS STREAM variable=imag_stream depth=32
#pragma HLS STREAM variable=mag_stream  depth=64
#pragma HLS STREAM variable=pk_bin      depth=2
#pragma HLS STREAM variable=pk_mag      depth=2
#pragma HLS STREAM variable=alert_code  depth=2

    adc_demux          (adc_stream, real_stream, imag_stream);
    magnitude_estimator(real_stream, imag_stream, mag_stream);
    peak_detector      (mag_stream, pk_bin, pk_mag);
    spoof_detector     (pk_bin, pk_mag, alert_code);
    alert_packer       (alert_code, alert_stream);
}

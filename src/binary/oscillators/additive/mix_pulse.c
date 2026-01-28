#include <math.h>

/**
 * Mix pulse oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 * @param negative_cycle_init 0x10000 - duty cycle
 */
void mix_pulse(int *osc_state, short *chunk_buffer, int chunk_len,
               int negative_cycle_init) {
  /*
   * Oscillator state
   */
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_phase_inc = osc_state[2];
  const int osc_vol = osc_state[7];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_detune = osc_state[20];
  const int osc_detune_m1 = osc_detune == 2 ? 1 : 0;
  const int osc_detune_phase =
      osc_phase_inc == osc_detune_phase_inc ? osc_phase : osc_phase_detuned;
  const int osc_amplitude = (osc_vol * 3) / 2;

  /*
   * Buffer constants
   */
  const double TWO_PI = M_PI * 2;
  const int negative_cycle =
      osc_buzz ? negative_cycle_init + 0x1800 : negative_cycle_init;
  const int L_NUM = 0x10000 / (0x10000 - negative_cycle);
  const int freq = (osc_phase_inc * 22050) >> 16;
  const int detune_freq = (osc_detune_phase_inc * 22050) >> 16;

  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 1; (i * freq) < 10914; i += 1) {
    if (i % L_NUM != 0) {
      normalize += 1.0 / i;
    }
  }

  double detune_normalize = 0;

  for (int i = 1; (i * detune_freq) < 10914; i += 1) {
    if (i % L_NUM != 0) {
      detune_normalize += 1.0 / i;
    }
  }

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;
  int cur_detune_phase = osc_detune_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    /*
     * Primary phasor
     */
    const double radians = ((double)cur_phase / 0x10000) * TWO_PI;

    double amplitude = 0;

    // 10914 = 11025 * 0.99
    for (int j = 1; (j * freq) < 10914; j += 1) {
      if (j % L_NUM != 0) {
        amplitude -= (1.0 / j) * sin(j * radians);
      }
    }

    /*
     * Detune phasor
     */
    const int detune_phase =
        osc_detune == 2 ? cur_detune_phase << 1 : cur_detune_phase;
    const double detune_radians =
        ((double)(detune_phase & 0xffff) / 0x10000) * TWO_PI;

    double detune_amplitude = 0;

    // 10914 = 11025 * 0.99
    for (int j = 1; (j * detune_freq) < 10914; j += 1) {
      if (j % L_NUM != 0) {
        detune_amplitude -= (1.0 / j) * sin(j * detune_radians);
      }
    }

    /*
     * Mix sample
     */
    amplitude = (amplitude / normalize) * 0x5ffc;
    detune_amplitude = (detune_amplitude / detune_normalize) * 0x2ffc;

    // Write new sample
    const int s_pregain = amplitude + detune_amplitude;
    const int s = (s_pregain * osc_amplitude) / 3072;
    chunk_buffer[i] = (short)s;

    // Increment phase
    cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
    cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0x1ffff;
  }

  // Update oscillator state
  osc_state[1] = cur_phase;
  osc_state[3] = cur_detune_phase;
}

#include <math.h>

/**
 * Mix triangle / phaser oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 */
void mix_triangle(int *osc_state, short *chunk_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_phase_inc = osc_state[2];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_vol = osc_state[7];
  const int osc_detune_phase = osc_phase_inc == osc_detune_phase_inc
                                   ? osc_phase
                                   : osc_phase_detuned;
  const int osc_amplitude = (osc_vol * 3) / 2;

  /*
   * Buffer constants
   */
  const double TWO_PI = M_PI * 2;
  const int freq = (osc_phase_inc * 22050) >> 16;
  const int detune_freq = (osc_detune_phase_inc * 22050) >> 16;

  double normalize = 0;

  // 10914 = 11025 * 0.99
  for (int i = 1; i * freq < 10914; i += 2) {
    double magnitude = 1.0 / (i * i);

    if (i & 3) {
      magnitude = -magnitude;
    }

    normalize += magnitude;
  }

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;
  int cur_detune_phase = osc_detune_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    const double radians = ((double)cur_phase / 0x10000) * TWO_PI;

    /*
     * Primary phasor
     */
    double amplitude = 0;

    // 10914 = 11025 * 0.99
    for (int j = 1; j * freq < 10914; j += 2) {
      double magnitude = 1.0 / (j * j);

      if ((j & 3) == 3) {
        magnitude = -magnitude;
      }

      amplitude += magnitude * sin(j * radians);
    }

    /*
     * Detune phasor
     */
    double detune_amplitude = 0;

    // 10914 = 11025 * 0.99
    for (int j = 1; j * detune_freq < 10914; j += 2) {
      double magnitude = 1.0 / (j * j);

      if ((j & 3) == 3) {
        magnitude = -magnitude;
      }

      detune_amplitude += magnitude * sin(j * radians);
    }

    /*
     * Buzz phasors
     *
     * @todo
     */
    // if (osc_buzz) {
    //   const int L_NUM = 0x10000 / (0x10000 - 0xdfff);
    //   const int buzz_phase = ((cur_phase * 0x5ffc) / 0x2000) & 0xffff;
    //   const double buzz_radians = ((double)buzz_phase / 0x10000) * TWO_PI;

    //   for (int j = 1; j * freq < 10914; j += 2) {
    //     double magnitude = 1.0 / (j * j);

    //     if ((j & 3) == 3 && j % L_NUM != 0) {
    //       magnitude = -magnitude;
    //     }

    //     amplitude += magnitude * sin(j * buzz_radians);
    //   }
    // }

    amplitude = (amplitude / normalize) * 0x3000;
    detune_amplitude = (detune_amplitude / normalize) * 0x1800;

    // Write new sample
    const int s_pregain = amplitude; //  + detune_amplitude;
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

/**
 * Mix white noise oscillator
 *
 * @see mix_noise()
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 */
void mix_white_noise(int *osc_state, short *chunk_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_pitch_decoded = osc_state[8] >> 16;
  const int osc_phase_inc = osc_state[2];
  const int osc_vol = osc_state[7];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_detune_phase = osc_phase_inc == osc_detune_phase_inc
                                   ? osc_phase
                                   : osc_phase_detuned;

  const int noiz = osc_state[22];
  const int osc_amplitude = (osc_vol * 3) / 2;
  const int osc_11 = osc_state[11];

  /*
   * Buffer state
   */
  int pitch_inverse = 64 - osc_pitch_decoded;
  pitch_inverse = pitch_inverse > 1 ? pitch_inverse : 1;
  pitch_inverse =
      pitch_inverse > 63 ? (pitch_inverse << 2) - 192 : pitch_inverse;

  /*
   * Populate buffer
   */
  int osc_11_accumulator = osc_11;

  for (int i = 0; i < chunk_len; i += 1) {
    int s;

    if (osc_11) {
      osc_11_accumulator = osc_11;

      if (noiz > 1) {
        const int x = (osc_11 * osc_amplitude) / pitch_inverse;
        const int y =
            osc_state[12] *
            (((pitch_inverse - osc_11) * osc_amplitude) / pitch_inverse);

        s = ((osc_state[13] * x) + y) / 2048;
      } else {
        s = (osc_state[12] * osc_amplitude) / 2048;
      }
    } else {
      osc_11_accumulator = osc_state[11];
      osc_state[12] = osc_state[13];
      osc_state[13] = codo_random(0x2ffe) - 0x17ff;

      s = (osc_state[12] * osc_amplitude) / 2048;
    }

    osc_state[11] = (osc_11_accumulator + 1) % pitch_inverse;

    // Write new sample
    chunk_buffer[i] = (short)s;
  }

  // Update oscillator state
  osc_state[1] = osc_phase;
  osc_state[3] = osc_detune_phase;
}

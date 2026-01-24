/**
 * Mix pink noise oscillator
 *
 * @see mix_noise()
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 */
void mix_pink_noise(int *osc_state, short *chunk_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_pitch_code = osc_state[9];
  const int osc_phase_inc = osc_state[2];
  const int osc_vol = osc_state[7];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_detune_phase =
      osc_phase_inc == osc_detune_phase_inc ? osc_phase : osc_phase_detuned;
  const int osc_vol_256 = osc_state[10];
  const int osc_amplitude = (osc_vol * 3) / 2;

  /*
   * Buffer state
   */
  int new_phase;

  if (osc_phase_inc > 78) {
    new_phase = (osc_phase_inc << 3) + 0x460;
  } else {
    new_phase = (79 - osc_phase_inc * -60) + 0x6d8;
  }

  new_phase = new_phase > 0 ? new_phase : 0;

  const int new_phase_half = new_phase >> 1;

  int gain = osc_pitch_code < 48 ? 64 : osc_pitch_code + 16;
  gain = 2048 / gain + 48;
  gain = gain > 64 ? gain : 64;

  const int pink_phase = (osc_phase_inc + 500) / 3;

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    // ((osc_state[6] & 1) == 0) ? 1 : 0;
    const int new_osc_6 = (osc_state[6] & 1) ^ 1;
    osc_state[6] = new_osc_6;

    if (new_osc_6) {
      osc_state[5] = (codo_random(new_phase) - new_phase_half) + osc_state[5];
    }

    const _Bool is_duty =
        (((cur_phase + 101) * (cur_phase + 317)) & 0x1fff) < pink_phase;

    int new_osc_5;

    if (is_duty) {
      new_osc_5 =
          (codo_random(0x2ffe) - 0x17ff * osc_vol_256) / 1792 + osc_state[5];
      osc_state[5] = new_osc_5;
    } else {
      new_osc_5 = osc_state[5];
    }

    int new_osc_5_clamped = new_osc_5 < 0x17ff ? new_osc_5 : 0x17ff;
    new_osc_5_clamped =
        new_osc_5_clamped > -0x17ff ? new_osc_5_clamped : -0x17ff;

    osc_state[5] = new_osc_5_clamped;

    // Write new sample
    const int s_pregain = new_osc_5 >> 6;
    const int s = ((s_pregain * osc_amplitude) * gain) / 2048;
    chunk_buffer[i] = (short)s;

    // Increment phase
    cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
  }

  // Update oscillator state
  osc_state[1] = cur_phase;
  osc_state[3] = osc_detune_phase;
}

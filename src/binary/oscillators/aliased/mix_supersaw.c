/**
 * Mix tilted sawtooth oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 */
void mix_supersaw(int *osc_state, short *chunk_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_detune = osc_state[20];
  const int osc_detune_m1 = osc_detune == 2 ? 1 : 0;
  const int osc_vol = osc_state[7];
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_phase_inc = osc_state[2];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_detune_phase = osc_phase_inc == osc_detune_phase_inc
                                   ? osc_phase
                                   : osc_phase_detuned;
  const int osc_amplitude = (osc_vol * 3) / 2;

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;
  int cur_detune_phase = osc_detune_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    const int detune_phase = cur_detune_phase << osc_detune_m1;
    const int detune_phase_partial = detune_phase & 0xffff;

    int amplitude;
    int detune_amplitude;

    if (osc_buzz) {
      const _Bool is_duty = cur_phase > 0xefff;
      const _Bool is_detune_duty = detune_phase_partial > 0xefff;

      amplitude = is_duty ? ((0xffff - cur_phase) * 0x5ffc) / 0x1000
                          : (cur_phase * 0x5ffc) / 0xf000;

      detune_amplitude = is_detune_duty
                             ? ((detune_phase_partial ^ 0xffff) * 0x5ffc) >> 12
                             : (detune_phase_partial * 0x5ffc) / 0xf000;
    } else {
      const _Bool is_duty = cur_phase > 0xdfff;
      const _Bool is_detune_duty = detune_phase_partial > 0xdfff;

      amplitude = is_duty ? ((0xffff - cur_phase) * 0x5ffc) / 0x2000
                          : (cur_phase * 0x5ffc) / 0xe000;

      detune_amplitude = is_detune_duty
                             ? ((detune_phase_partial ^ 0xffff) * 0x5ffc) >> 13
                             : (detune_phase_partial * 0x5ffc) / 0xe000;
    }

    // Write buffer
    const int s_pregain = amplitude - 0x2ffe + (detune_amplitude - 0x2ffe) / 2;
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

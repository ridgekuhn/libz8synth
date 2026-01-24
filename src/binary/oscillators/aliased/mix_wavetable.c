/**
 * Mix wavetable frame
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 */
void mix_wavetable(int *osc_state, short *chunk_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const int osc_detune = osc_state[20];
  const int osc_detune_m1 = osc_detune == 2 ? 1 : 0;
  const int osc_phase = osc_state[1];
  const int osc_phase_detuned = osc_state[3];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_phase_inc = osc_state[2];
  const int osc_vol = osc_state[7];
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
    const int cur_frame_s = osc_state[24 + ((cur_phase >> 10) & 63)];
    const int next_frame_s = osc_state[24 + (((cur_phase + 0x400) >> 10) & 63)];

    // Get wavetable frame detune samples
    const int detune_phase = cur_detune_phase << osc_detune_m1;
    const int detune_s = osc_state[24 + ((detune_phase >> 10) & 63)];
    const int next_detune_s =
        osc_state[24 + (((detune_phase + 1024) >> 10) & 63)];

    // Calculate new sample
    const int amplitude = (((next_frame_s - cur_frame_s) * (cur_phase & 1023)) +
                           (cur_frame_s << 10)) >>
                          10;
    const int detune_amplitude =
        (((next_detune_s - detune_s) * (detune_phase & 1023)) +
         (detune_s << 10)) >>
        10;

    // Write new sample
    const int s_pregain = amplitude + detune_amplitude / 2;
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

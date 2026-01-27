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
  const int osc_phase_detuned = osc_state[3];
  const int osc_detune = osc_state[20];
  const int osc_vol = osc_state[7];
  const int osc_phase = osc_state[1];
  const int osc_phase_inc = osc_state[2];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_detune_phase =
      osc_phase_inc == osc_detune_phase_inc ? osc_phase : osc_phase_detuned;
  const int osc_amplitude = (osc_vol * 3) / 2;

  /*
   * Buffer state
   */
  const int negative_cycle =
      osc_buzz ? negative_cycle_init + 0x1800 : negative_cycle_init;

  /*
   * Populate buffer
   */
  int cur_phase = osc_phase;
  int cur_detune_phase = osc_detune_phase;

  for (int i = 0; i < chunk_len; i += 1) {
    const _Bool is_negative = cur_phase < negative_cycle;

    int amplitude = is_negative ? -0x17ff : 0x17ff;
    amplitude += (-amplitude * polyblep((cur_phase + negative_cycle) & 0xffff,
                                       osc_phase_inc)) >>
                 16;
    amplitude -= (amplitude * polyblep(cur_phase, osc_phase_inc)) >> 16;

    const detune_partial = cur_detune_phase & -0xffff;

    const _Bool is_detune_negative =
        osc_detune == 2 ? ((cur_detune_phase << 1) & 0xfffe) < negative_cycle
                        : detune_partial < negative_cycle;

    int detune_amplitude = is_detune_negative ? -0xbff : 0xbff;
    detune_amplitude += (-detune_amplitude *
                         polyblep((detune_partial + negative_cycle) & 0xffff,
                                  osc_detune_phase_inc)) >>
                        16;
    detune_amplitude -=
        (detune_amplitude * polyblep(detune_partial, osc_detune_phase_inc)) >>
        16;

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

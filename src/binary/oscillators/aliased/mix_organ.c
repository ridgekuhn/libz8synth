/**
 * Mix organ oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 */
void mix_organ(int *osc_state, short *chunk_buffer, int chunk_len) {
  /*
   * Oscillator state
   */
  const int osc_phase = osc_state[1];
  const int osc_phase_inc = osc_state[2];
  const int osc_phase_detuned = osc_state[3];
  const int osc_detune_phase_inc = osc_state[4];
  const int osc_vol = osc_state[7];
  const int osc_detune = osc_state[20];
  const _Bool osc_buzz = osc_state[21] != 0;
  const int osc_detune_m1 = osc_detune == 2 ? 1 : 0;
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
    const _Bool is_duty = (cur_phase & 0x8000) == 0;
    const _Bool is_sub_duty = (cur_phase & 0x4000) != 0;

    int amplitude;

    if (is_duty) {
      if (is_sub_duty) {
        amplitude = 0x8000 - cur_phase;
      } else {
        amplitude = cur_phase;
      }
    } else {
      if (is_sub_duty) {
        amplitude = 0x10000 - cur_phase;
      } else {
        amplitude = cur_phase - 0x8000;
      }

      amplitude = (amplitude << 1) / 3;
    }

    int detune_amplitude;

    if (osc_buzz) {
      const int buzz_detune = 0x8000 >> osc_detune_m1;
      const _Bool is_detune_duty = (cur_detune_phase & buzz_detune) == 0;

      detune_amplitude = is_detune_duty ? -0x5ff : 0x5ff;
    } else {
      const int detune = cur_detune_phase << osc_detune_m1;
      const int detune_partial = detune & 0xffff;
      const _Bool is_detune_duty = (detune & 0x8000) == 0;
      const _Bool is_detune_sub_duty = (detune & 0x4000) != 0;

      if (is_detune_duty) {
        if (is_detune_sub_duty) {
          detune_amplitude = 0x8000 - detune_partial;
        } else {
          detune_amplitude = detune_partial;
        }
      } else {
        if (is_detune_sub_duty) {
          detune_amplitude = 0x10000 - detune_partial;
        } else {
          detune_amplitude = detune_partial - 0x8000;
        }

        detune_amplitude = (detune_amplitude << 1) / 3;
      }

      detune_amplitude = (detune_amplitude - 0x2000) / 2;
    }

    // Write new sample
    const int s_pregain = amplitude + detune_amplitude - 0x2000;
    const int s = (s_pregain * osc_amplitude) / 3072;
    chunk_buffer[i] = (short)s;

    // Increment phase
    cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
    cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0xffff;
  }

  // Update oscillator state
  osc_state[1] = cur_phase;
  osc_state[3] = cur_detune_phase;
}

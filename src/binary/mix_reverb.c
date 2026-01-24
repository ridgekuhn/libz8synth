/**
 * Mix reverb
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 * @param ch_state Mixer channel state
 */
void mix_reverb(int *osc_state, short *chunk_buffer, int chunk_len,
                long ch_state) {
  /*
   * Oscillator state
   */
  const int osc_reverb = osc_state[23];

  // Nothing to do
  if (osc_reverb <= 0) {
    return;
  }

  // 0x2d20 = 0x2d14 (-0xc) in asm.js export
  const int cur_pat_tick = *(int *)(ch_state + 0x2d20);
  const int reverb_ticks = osc_reverb * 2;
  const int reverb_offset = (cur_pat_tick - reverb_ticks) & 7;

  /*
   * Apply buffer reverb
   */
  for (int i = 0; i < chunk_len; i += 1) {
    const short sample = chunk_buffer[i];
    // 0x21ae = 0x21a2 (-0xc) in asm.js export
    const short reverb_sample =
        *(short *)(ch_state + 0x21ae + reverb_offset * 366 + (i << 1));
    const int s = ((reverb_sample << 1) + (sample << 2)) / 4;

    chunk_buffer[i] = (short)s;
  }
}

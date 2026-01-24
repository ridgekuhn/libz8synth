/**
 * Mix oscillator tick
 *
 * Mix oscillator state to mixer channel buffer
 *
 * @see mix_wavetable()
 * @see mix_triangle()
 * @see mix_supersaw()
 * @see mix_sawtooth()
 * @see mix_pulse()
 * @see mix_organ()
 * @see mix_noise()
 * @see mix_reverb()
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in samples
 * @param ch_state Mixer channel state
 */
void mix_osc_tick(int *osc_state, short *chunk_buffer, int chunk_len,
                  long ch_state) {
  // Nothing to do
  if (chunk_len <= 0) {
    return;
  }

  /*
   * Oscillator state
   */
  const int osc_vol = osc_state[7];
  const int waveform = *osc_state;

  // If no audio to mix, zero-out buffer and return early
  // 0x2ee4 = 0x2ed8 (-0xc) in asm.js export
  if (osc_vol == 0 && *(int *)(ch_state + 0x2ee4) == 0) {
    osc_state[1] = 0;
    memset(chunk_buffer, 0, chunk_len << 1);
    return;
  }

  /*
   * Select and mix oscillator
   */
  // Wavetable frame
  if (waveform == 8) {
    mix_wavetable(osc_state, chunk_buffer, chunk_len);
  }

  // Triangle, phaser waveforms
  if (waveform == 0 || waveform == 7) {
    mix_triangle(osc_state, chunk_buffer, chunk_len);
  }

  // Tilted sawtooth waveform
  if (waveform == 1) {
    mix_supersaw(osc_state, chunk_buffer, chunk_len);
  }

  // Sawtooth waveform
  if (waveform == 2) {
    mix_sawtooth(osc_state, chunk_buffer, chunk_len);
  }

  // Square waveform
  if (waveform == 3) {
    mix_pulse(osc_state, chunk_buffer, chunk_len, 0x8000);
  }

  // 25% duty cycle pulse waveform
  if (waveform == 4) {
    mix_pulse(osc_state, chunk_buffer, chunk_len, 0xb000);
  }

  // Organ waveform
  if (waveform == 5) {
    mix_organ(osc_state, chunk_buffer, chunk_len);
  }

  // Noise waveform
  if (waveform == 6) {
    mix_noise(osc_state, chunk_buffer, chunk_len);
  }

  /*
   * Apply buffer reverb
   */
  mix_reverb(osc_state, chunk_buffer, chunk_len, ch_state);
}

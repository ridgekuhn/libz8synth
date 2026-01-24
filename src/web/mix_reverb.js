/**
 * Mix reverb
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in bytes
 * @param ch_state Mixer channel state
 */
function mix_reverb(osc_state, chunk_buffer, chunk_len, ch_state) {
	/*
	 * Oscillator state
	 */
	const osc_reverb = c[(osc_state + 92) >> 2];

	// Nothing to do
	if (osc_reverb <= 0) {
		return;
	}

	// 0x2d14 = 0x2d20 (+0xc) in p8 binary
	const cur_pat_tick = c[(ch_state + 0x2d14) >> 2];
	const reverb_ticks = osc_reverb * 2;
	const reverb_offset = (cur_pat_tick - reverb_ticks) & 7;

	/*
	 * Apply buffer reverb
	 */
	for (let i = 0; i < chunk_len; i += 1) {
		const sample_addr = chunk_buffer + (i << 1);
		const sample = b[sample_addr >> 1];
		// 0x21a2 = 0x21ae (+0xc) in p8 binary
		const reverb_addr = ch_state + 0x21a2 + reverb_offset * 366 + (i << 1);
		const reverb_sample = b[reverb_addr >> 1];
		const s = ((reverb_sample << 1) + (sample << 2)) / 4;

		b[sample_addr] = s;
	}
}

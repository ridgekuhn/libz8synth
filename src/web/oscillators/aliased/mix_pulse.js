/**
 * Mix pulse oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in bytes
 * @param negative_cycle_init 0x10000 - duty cycle
 */
function mix_pulse(osc_state, chunk_buffer, chunk_len, negative_cycle_init) {
	/*
	 * Oscillator state
	 */
	const osc_buzz = c[(osc_state + 84) >> 2] != 0;
	const osc_detune = c[(osc_state + 80) >> 2];
	const osc_vol = c[(osc_state + 28) >> 2];
	const osc_phase_addr = osc_state + 4;
	const osc_phase = c[osc_phase_addr >> 2];
	const osc_phase_detuned_addr = osc_state + 12;
	const osc_phase_inc = c[(osc_state + 8) >> 2];
	const osc_detune_phase_inc = c[(osc_state + 16) >> 2];
	const osc_detune_phase =
		osc_phase_inc == osc_detune_phase_inc
			? osc_phase
			: c[osc_phase_detuned_addr >> 2];
	const osc_amplitude = (osc_vol * 3) / 2;

	/*
	 * Buffer state
	 */
	const negative_cycle = osc_buzz
		? negative_cycle_init + 0x1800
		: negative_cycle_init;

	/*
	 * Populate buffer
	 */
	let cur_phase = osc_phase;
	let cur_detune_phase = osc_detune_phase;

	for (let i = 0; i < chunk_len; i += 1) {
		const is_negative = cur_phase < negative_cycle;
		const amplitude = is_negative ? -0x17ff : 0x17ff;
		const is_detune_negative =
			osc_detune == 2
				? ((cur_detune_phase << 1) & 0xfffe) >>> 0 < negative_cycle
				: (cur_detune_phase & 0xffff) >>> 0 < negative_cycle;
		const detune_amplitude = is_detune_negative ? -0xbff : 0xbff;

		// Write new sample
		const s_pregain = amplitude + detune_amplitude;
		const s = B(s_pregain, osc_amplitude) / 3072;
		b[(chunk_buffer + (i << 1)) >> 1] = s;

		// Increment phase
		cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
		cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0x1ffff;
	}

	// Update oscillator state
	c[osc_phase_addr >> 2] = cur_phase;
	c[osc_phase_detuned_addr >> 2] = cur_detune_phase;
}

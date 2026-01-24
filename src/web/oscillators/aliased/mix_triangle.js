/**
 * Mix triangle / phaser oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in bytes
 */
function mix_triangle(osc_state, chunk_buffer, chunk_len) {
	/*
	 * Oscillator state
	 */
	const osc_buzz = c[(osc_state + 84) >> 2] != 0;
	const osc_phase_addr = osc_state + 4;
	const osc_phase = c[osc_phase_addr >> 2];
	const osc_phase_detuned_addr = osc_state + 12;
	const osc_phase_inc = c[(osc_state + 8) >> 2];
	const osc_detune_phase_inc = c[(osc_state + 16) >> 2];
	const osc_vol = c[(osc_state + 28) >> 2];
	const osc_detune_phase =
		osc_phase_inc == osc_detune_phase_inc
			? osc_phase
			: c[osc_phase_detuned_addr >> 2];
	const osc_amplitude = (osc_vol * 3) / 2;

	/*
	 * Populate buffer
	 */
	let cur_phase = osc_phase;
	let cur_detune_phase = osc_detune_phase;

	for (let i = 0; i < chunk_len; i += 1) {
		const is_duty = (cur_phase & 0x8000) == 0;
		const is_detune_duty = (cur_detune_phase & 0x8000) == 0;
		const cur_detune_partial = cur_detune_phase & 0xffff;

		let amplitude = is_duty ? cur_phase * 3 - 0xc000 : (0xc000 - cur_phase) * 3;

		let detune_amplitude = is_detune_duty
			? cur_detune_partial * 3 - 0xc000
			: (0xc000 - cur_detune_partial) * 3;

		if (osc_buzz) {
			const is_buzz_duty = cur_phase > 0xdfff;
			const is_buzz_detune_duty = cur_detune_partial >>> 0 > 0xdfff;

			const buzz_amp = is_buzz_duty
				? ((0xffff - cur_phase) * 0x5ffc) / 0x2000
				: (cur_phase * 0x5ffc) / 0xe000;

			const detune_buzz_amp = is_buzz_detune_duty
				? ((cur_detune_partial ^ 0xffff) * 0x5ffc) >>> 13
				: ((cur_detune_partial * 0x5ffc) >>> 0) / 0xe000;

			amplitude = (amplitude / 4) * 3 - 0x2ffe + buzz_amp;
			detune_amplitude = (detune_amplitude / 4) * 3 - 0x2ffe + detune_buzz_amp;
		}

		// Write buffer
		const s_pregain = amplitude / 4 + detune_amplitude / 8;
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

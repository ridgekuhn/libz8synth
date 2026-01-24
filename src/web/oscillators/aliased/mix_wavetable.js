/**
 * Mix wavetable frame
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in bytes
 */
function mix_wavetable(osc_state, chunk_buffer, chunk_len) {
	/*
	 * Oscillator state
	 */
	const osc_detune = c[(osc_state + 80) >> 2];
	const osc_detune_m1 = osc_detune == 2 ? 1 : 0;
	const osc_phase_addr = osc_state + 4;
	const osc_phase = c[osc_phase_addr >> 2];
	const osc_phase_detuned_addr = osc_state + 12;
	const osc_detune_phase_inc = c[(osc_state + 16) >> 2];
	const osc_phase_inc = c[(osc_state + 8) >> 2];
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
		const cur_frame_s =
			c[(osc_state + 96 + (((cur_phase >>> 10) & 63) << 2)) >> 2];
		const next_frame_s =
			c[(osc_state + 96 + ((((cur_phase + 1024) >>> 10) & 63) << 2)) >> 2];

		// Get wavetable frame detune samples
		const detune_phase = cur_detune_phase << osc_detune_m1;
		const detune_s =
			c[(osc_state + 96 + (((detune_phase >>> 10) & 63) << 2)) >> 2];
		const next_detune_s =
			c[(osc_state + 96 + ((((detune_phase + 1024) >>> 10) & 63) << 2)) >> 2];

		// Write new sample
		const amplitude =
			(B(next_frame_s - cur_frame_s, cur_phase & 1023) + (cur_frame_s << 10)) >>
			10;
		const detune_amplitude =
			(B(next_detune_s - detune_s, detune_phase & 1023) + (detune_s << 10)) >>
			10;

		// Write buffer
		const s_pregain = amplitude + detune_amplitude / 2;
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

/**
 * Mix pink noise oscillator
 *
 * @see mix_noise()
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in bytes
 */
function mix_pink_noise(osc_state, chunk_buffer, chunk_len) {
	/*
	 * Oscillator state
	 */
	const osc_5_addr = osc_state + 20;
	const osc_6_addr = osc_state + 24;
	const osc_phase_addr = osc_state + 4;
	const osc_phase = c[osc_phase_addr >> 2];
	const osc_phase_detuned_addr = osc_state + 12;
	const osc_pitch_code = c[(osc_state + 36) >> 2];
	const osc_phase_inc = c[(osc_state + 8) >> 2];
	const osc_vol = c[(osc_state + 28) >> 2];
	const osc_detune_phase_inc = c[(osc_state + 16) >> 2];
	const osc_detune_phase =
		osc_phase_inc == osc_detune_phase_inc
			? osc_phase
			: c[osc_phase_detuned_addr >> 2];
	const osc_vol_256 = c[(osc_state + 40) >> 2];
	const osc_amplitude = (osc_vol * 3) / 2;

	/*
	 * Buffer state
	 */
	let new_phase;

	if (osc_phase_inc > 78) {
		new_phase = (osc_phase_inc << 3) + 0x460;
	} else {
		new_phase = B(79 - osc_phase_inc, -60) + 0x6d8;
	}

	new_phase = new_phase > 0 ? new_phase : 0;

	const new_phase_half = new_phase >>> 1;

	let gain = osc_pitch_code < 48 ? 64 : osc_pitch_code + 16;
	gain = 2048 / gain + 48;
	gain = gain > 64 ? gain : 64;

	const pink_phase = (osc_phase_inc + 500) / 3;

	/*
	 * Populate buffer
	 */
	let cur_phase = osc_phase;

	for (let i = 0; i < chunk_len; i += 1) {
		// ((c[osc_6_addr >> 2] & 1) == 0) ? 1 : 0;
		const new_osc_6 = (c[osc_6_addr >> 2] & 1) ^ 1;
		c[osc_6_addr >> 2] = new_osc_6;

		if (new_osc_6) {
			c[osc_5_addr >> 2] = tp(new_phase) - new_phase_half + c[osc_5_addr >> 2];
		}

		const is_duty = (B(cur_phase + 101, cur_phase + 317) & 0x1fff) < pink_phase;

		let new_osc_5;

		if (is_duty) {
			new_osc_5 =
				B(tp(0x2ffe) - 0x17ff, osc_vol_256) / 1792 + c[osc_5_addr >> 2];
			c[osc_5_addr >> 2] = new_osc_5;
		} else {
			new_osc_5 = c[osc_5_addr >> 2];
		}

		let new_osc_5_clamped = new_osc_5 < 0x17ff ? new_osc_5 : 0x17ff;
		new_osc_5_clamped =
			new_osc_5_clamped > -0x17ff ? new_osc_5_clamped : -0x17ff;

		c[osc_5_addr >> 2] = new_osc_5_clamped;

		// Write new sample
		const s_pregain = new_osc_5 >> 6;
		const s = B(B(s_pregain, osc_amplitude), gain) / 2048;
		b[(chunk_buffer + (i << 1)) >> 1] = s;

		// Increment phase
		cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
	}

	// Update oscillator state
	c[osc_phase_addr >> 2] = cur_phase;
	c[osc_phase_detuned_addr >> 2] = osc_detune_phase;
}

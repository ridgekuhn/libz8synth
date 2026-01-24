/**
 * Mix white noise oscillator
 *
 * @see mix_noise()
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in bytes
 */
function mix_white_noise(osc_state, chunk_buffer, chunk_len) {
	/*
	 * Oscillator state
	 */
	const osc_11_addr = osc_state + 44;
	const osc_12_addr = osc_state + 48;
	const osc_13_addr = osc_state + 52;
	const osc_phase_addr = osc_state + 4;
	const osc_phase = c[osc_phase_addr >> 2];
	const osc_phase_detuned_addr = osc_state + 12;
	const osc_pitch_decoded = c[(osc_state + 32) >> 2] >> 16;
	const osc_phase_inc = c[(osc_state + 8) >> 2];
	const osc_vol = c[(osc_state + 28) >> 2];
	const osc_detune_phase_inc = c[(osc_state + 16) >> 2];
	const osc_detune_phase =
		osc_phase_inc == osc_detune_phase_inc
			? osc_phase
			: c[osc_phase_detuned_addr >> 2];

	const noiz = c[(osc_state + 88) >> 2];
	const osc_amplitude = (osc_vol * 3) / 2;
	const osc_11 = c[osc_11_addr >> 2];

	/*
	 * Buffer state
	 */
	let pitch_inverse = 64 - osc_pitch_decoded;
	pitch_inverse = pitch_inverse > 1 ? pitch_inverse : 1;
	pitch_inverse =
		pitch_inverse > 63 ? (pitch_inverse << 2) - 192 : pitch_inverse;

	/*
	 * Populate buffer
	 */
	let osc_11_accumulator = osc_11;

	for (let i = 0; i < chunk_len; i += 1) {
		let s;

		if (osc_11) {
			osc_11_accumulator = osc_11;

			if (noiz > 1) {
				const x = B(osc_11, osc_amplitude) / pitch_inverse;
				const y = B(
					c[osc_12_addr >> 2],
					B(pitch_inverse - osc_11, osc_amplitude) / pitch_inverse,
				);

				s = (B(c[osc_13_addr >> 2], x) + y) / 2048;
			} else {
				s = B(c[osc_12_addr >> 2], osc_amplitude) / 2048;
			}
		} else {
			osc_11_accumulator = c[osc_11_addr >> 2];
			c[osc_12_addr >> 2] = c[osc_13_addr >> 2];
			c[osc_13_addr >> 2] = tp(0x2ffe) - 0x17ff;

			s = B(c[osc_12_addr >> 2], osc_amplitude) / 2048;
		}

		c[osc_11_addr >> 2] = (osc_11_accumulator + 1) % pitch_inverse;

		// Write new sample
		b[(chunk_buffer + (i << 1)) >> 1] = s;
	}

	// Update oscillator state
	c[osc_phase_addr >> 2] = osc_phase;
	c[osc_phase_detuned_addr >> 2] = osc_detune_phase;
}

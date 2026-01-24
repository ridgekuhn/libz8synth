/**
 * Mix organ oscillator
 *
 * @param osc_state Mixer channel oscillator state
 * @param chunk_buffer Mixer channel chunk buffer
 * @param chunk_len Length of chunk_buffer in bytes
 */
function mix_organ(osc_state, chunk_buffer, chunk_len) {
	/*
	 * Oscillator state
	 */
	const osc_phase_addr = osc_state + 4;
	const osc_phase = c[osc_phase_addr >> 2];
	const osc_phase_inc = c[(osc_state + 8) >> 2];
	const osc_phase_detuned_addr = osc_state + 12;
	const osc_detune_phase_inc = c[(osc_state + 16) >> 2];
	const osc_vol = c[(osc_state + 28) >> 2];
	const osc_detune = c[(osc_state + 80) >> 2];
	const osc_buzz = c[(osc_state + 84) >> 2] != 0;
	const osc_detune_m1 = osc_detune == 2 ? 1 : 0;
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
		const is_sub_duty = (cur_phase & 0x4000) != 0;

		let amplitude;

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

		let detune_amplitude;

		if (osc_buzz) {
			const buzz_detune = 0x8000 >>> osc_detune_m1;
			const is_detune_duty = (cur_detune_phase & buzz_detune) == 0;

			detune_amplitude = is_detune_duty ? -0x5ff : 0x5ff;
		} else {
			const detune = cur_detune_phase << osc_detune_m1;
			const detune_partial = detune & 0xffff;
			const is_detune_duty = (detune & 0x8000) == 0;
			const is_detune_sub_duty = (detune & 0x4000) != 0;

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
		const s_pregain = amplitude + detune_amplitude - 0x2000;
		const s = B(s_pregain, osc_amplitude) / 3072;
		b[(chunk_buffer + (i << 1)) >> 1] = s;

		// Increment phase
		cur_phase = (cur_phase + osc_phase_inc) & 0xffff;
		cur_detune_phase = (cur_detune_phase + osc_detune_phase_inc) & 0xffff;
	}

	// Update oscillator state
	c[osc_phase_addr >> 2] = cur_phase;
	c[osc_phase_detuned_addr >> 2] = cur_detune_phase;
}

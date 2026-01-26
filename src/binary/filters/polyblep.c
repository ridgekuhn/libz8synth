/**
 * PolyBLEP
 *
 * Adapted from:
 * https://www.kvraudio.com/forum/viewtopic.php?t=37550x100007
 *
 * @param t Oscillator phase, 0 <= t < 0x10000
 * @param dt Phase increment, (frequency << 16) / sample_rate
 */
int polyblep(int t, int dt) {
  // 0 <= t < 0x10000
  if (t < dt) {
    t /= dt;
    // 2 * (t - t^2/2 - 0.5)
    return t + t - ((t * t) >> 16) - 0x10000;
    // -0x10000 < t < 0
  } else if (t > 0x10000 - dt) {
    t = (t - 0x10000) / dt;
    // 2 * (t^2/2 + t + 0.5)
    return ((t * t) >> 16) + t + t + 0x10000;
    // 0 otherwise
  } else {
    return 0;
  }
}

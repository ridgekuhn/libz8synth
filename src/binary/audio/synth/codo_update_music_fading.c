#include "../../globals.h"
#include "../../time/codo_get_time.h"
#include "../codo_lock_audio_plat.h"
#include "../codo_unlock_audio_plat.h"
#include "./codo_stop_pico8_song.h"

/**
 * Update music fade
 */
void codo_update_music_fading() {
  codo_lock_audio_plat();

  if (fade_vol == fade1) {
    if (fade_vol == 0) {
      music_playing = 0;
      codo_stop_pico8_song(0);
    }

    codo_unlock_audio_plat();

    return;
  }

  if (fade_len == 0) {
    fade_vol = fade1;

    if (fade1 == 0) {
      music_playing = 0;
      codo_stop_pico8_song(0);
    }

    codo_unlock_audio_plat();

    return;
  }

  const int fade_progress = codo_get_time() - fade_start_t;

  if (fade_len > fade_progress) {
    int new_fade_vol =
        ((fade1 * fade_progress) + (fade0 * (fade_len - fade_progress))) /
        fade_len;
    new_fade_vol = new_fade_vol < 65536 ? new_fade_vol : 65536;
    new_fade_vol = new_fade_vol > 0 ? new_fade_vol : 0;

    fade_vol = new_fade_vol;

    codo_unlock_audio_plat();

    return;
  }

  fade_vol = fade1;

  if (fade1 == 0) {
    music_playing = 0;
    codo_stop_pico8_song(0);
  }

  codo_unlock_audio_plat();
}

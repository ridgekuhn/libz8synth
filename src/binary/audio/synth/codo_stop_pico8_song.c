#include "../../globals.h"
#include "../codo_lock_audio_plat.h"
#include "../codo_unlock_audio_plat.h"
#include "./stop_ch.h"

void codo_stop_pico8_song(long *cart_ptr) {
  codo_lock_audio_plat();

  if (cart_ptr) {
    for (int i = 0; i < 16; i += 1) {
      long *ch_state = (long *)ms0 + i * 0x3700;
      const long *ch_cart_ptr = (ch_state + 0x2020);

      if (*(ch_state + 0x2d28) && ch_cart_ptr != 0 && ch_cart_ptr == cart_ptr) {
        stop_ch(ch_state);
      }
    }
  } else {
    for (int i = 0; i < 16; i += 1) {
      long *ch_state = (long *)ms0 + i * 0x3700;

      if (*(ch_state + 0x2d28)) {
        stop_ch(ch_state);
      }
    }
  }

  codo_unlock_audio_plat();
}

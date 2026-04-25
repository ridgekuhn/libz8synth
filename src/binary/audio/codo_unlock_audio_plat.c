#include "../globals.h"
#include <SDL.h>

/**
 * Unlock SDL audio device
 */
void codo_unlock_audio_plat() {
  codo_audio_is_locked -= 1;

  if (codo_audio_is_locked == 0) {
    SDL_UnlockAudio();
  }
}

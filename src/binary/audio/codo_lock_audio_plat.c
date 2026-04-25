#include "../globals.h"
#include <SDL.h>

/**
 * Lock SDL audio device
 */
void codo_lock_audio_plat() {
  if (codo_audio_is_locked == 0) {
    SDL_LockAudio();
  }

  codo_audio_is_locked += 1;
}

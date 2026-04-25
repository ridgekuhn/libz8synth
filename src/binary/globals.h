#include "./cart/cdata.h"
#include <stdbool.h>
#include <stdint.h>

/*
 * Named globals
 */
#ifndef advanced_pattern
extern _Bool advanced_pattern;
#endif

#ifndef codo_audio_buffer
extern int8_t codo_audio_buffer[0x55f00];
#endif

#ifndef codo_audio_is_locked
extern int codo_audio_is_locked;
#endif

#ifndef codo_state
extern int codo_state[172];
#endif

#ifndef inside_codo_mixer_callback
extern _Bool inside_codo_mixer_callback;
#endif

#ifndef last_callback_len
extern int last_callback_len;
#endif

#ifndef m_low
extern int m_low;
#endif

#ifndef m_high
extern int m_high;
#endif

#ifndef ms0
extern int8_t ms0[0x37000];
#endif

#ifndef fade_vol
extern int fade_vol;
#endif

/*
 * Custom globals
 */
#ifndef GLOBALS
#define GLOBALS

extern int audio_clock_mask;
extern int bytes_per_tick;
extern int global_bitcrush;
extern int global_dampen;
extern int global_reverb;
extern int music_volume;
extern CData *g_cdata;
extern int g_version;
extern int SAMPLES_PER_TICK;
extern int sound_initialized;
extern int sound_volume;

#endif

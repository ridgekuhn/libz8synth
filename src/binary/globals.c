#include "./globals.h"
#include "./cart/cdata.h"

/*
 * Named globals
 */
_Bool advanced_pattern = 0;
int8_t codo_audio_buffer[0x55f00];
int codo_audio_is_locked = 0;

int codo_state[172] = {
    // is_paused
    [604 >> 2] = 0,
    // chunks mixed
    [608 >> 2] = 0,
    // audio playing
    [612 >> 2] = 0,
    // codo_audio_buffer_init
    [632 >> 2] = 0,
    // patterns_played
    [636 >> 2] = 0,
    // cur_pat_idx
    [640 >> 2] = 0,
    // prog_sample_rate = 22050,
    [660 >> 2] = 0,
    // prog_channels
    [664 >> 2] = 0,
    // sdl_sample_rate
    [668 >> 2] = 0,
    // sdl_channels
    [672 >> 2] = 0,
};

_Bool inside_codo_mixer_callback = 0;
int last_callback_len = 0;
int m_high = 0x1234567;
int m_low = 0xdeadbeef;
int8_t ms0[0x37000];

/*
 * Custom globals
 */
int audio_clock_mask = 0;
int bytes_per_tick = 366;
int global_bitcrush = 0;
int global_dampen = 0;
int global_reverb = 0;
CData *g_cdata = 0;
int g_version = 43;
int music_volume = 256;
int SAMPLES_PER_TICK = 183;
int sound_initialized = 0;
int sound_volume = 256;

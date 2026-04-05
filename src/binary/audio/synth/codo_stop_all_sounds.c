#include <stdint.h>
#include "../../globals.h"

void codo_stop_all_sounds(void) {
  // @TODO Lock SDL if implemented

  for (int i = 0; i < 16; i += 1) {
    int8_t *ch_state = *(&ms0 + 0x3700 * i);

    *(ch_state + 0x2010) = 0;
    *(ch_state + 0x2020) = 0;
    *(ch_state + 0x2028) = 0;
    *(ch_state + 0x2034) = 0;
    *(ch_state + 0x2d28) = 0;
  }
}

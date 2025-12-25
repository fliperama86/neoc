#include <stdlib.h>

#include "neoc.h"

neoc_t *neoc_create(void) {
  neoc_t *emu = calloc(1, sizeof(neoc_t));
  emu->m68k = m68k_create();
  emu->bus = bus_create();

  neoc_reset(emu);
  return emu;
}

void neoc_destroy(neoc_t *emu) {
  if (!emu)
    return;

  m68k_destroy(emu->m68k);
  bus_destroy(emu->bus);
  free(emu);
}

void neoc_reset(neoc_t *emu) {
  if (!emu)
    return;

  m68k_reset(emu->m68k);
  bus_reset(emu->bus);

  emu->running = true;
  emu->frame_count = 0;

  m68k_reset(emu->m68k);
}

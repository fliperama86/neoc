#include <stdlib.h>

#include "neoc.h"

neoc_t *neoc_create(void) {
  neoc_t *emu = calloc(1, sizeof(neoc_t));
  return emu;
}

void neoc_destroy(neoc_t *emu) { free(emu); }

void neoc_reset(neoc_t *emu) {
  if (!emu)
    return;

  m68k_reset(emu->m68k);
}

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "bus.h"
#include "m68k.h"
typedef struct neoc {
  m68k_cpu_t *m68k;
  bus_t *bus;

  bool running;
  int frame_count;
} neoc_t;

neoc_t *neoc_create(void);
void neoc_destroy(neoc_t *emu);
void neoc_reset(neoc_t *emu);

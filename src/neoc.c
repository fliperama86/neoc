#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "m68k.h"

struct m68k_cpu {
  uint32_t d[8];
  uint32_t a[8];
  uint32_t pc;
  uint16_t sr;
  uint32_t usp;
  uint32_t ssp;

  bool stopped;
  long cycles;
  uint32_t pending_interrupts;
};

m68k_cpu_t *m68k_create(void) {
  m68k_cpu_t *cpu = calloc(1, sizeof(m68k_cpu_t));
  return cpu;
}

void m68k_destroy(m68k_cpu_t *cpu) { free(cpu); }

void m68k_reset(m68k_cpu_t *cpu) {
  if (!cpu)
    return;

  memset(cpu->d, 0, sizeof(cpu->d));
  memset(cpu->a, 0, sizeof(cpu->a));
  cpu->pc = 0;
  cpu->sr = 0;
  cpu->usp = 0;
  cpu->ssp = 0;
  cpu->stopped = false;
  cpu->cycles = 0;
  cpu->pending_interrupts = 0;
}

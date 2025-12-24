#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bus.h"
#include "m68k.h"

int main() {
  m68k_cpu_t *cpu = m68k_create();
  m68k_reset(cpu);
  printf("Hello, World!\n");
  m68k_destroy(cpu);

  return 0;
}

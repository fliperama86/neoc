#include "bus.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint32_t d[8]; // Data registers D0-D7
  uint32_t a[8]; // Address registers A0-A7 (A7 is SP)
  uint32_t pc;   // Program Counter
  uint16_t sr;   // Status Register

  // Internal emulator state
  bool stopped; // For STOP instruction
  long cycles;  // Clock cycle counter
  uint32_t pending_interrupts;
} m68k_context_t;

m68k_context_t m68k_cpu;

int main() {
  printf("Hello, World!\n");
  return 0;
}

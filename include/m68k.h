#include <stdbool.h>
#include <stdint.h>

#define SR_C 0x01
#define SR_V 0x02
#define SR_Z 0x04
#define SR_N 0x08
#define SR_X 0x10

typedef struct m68k_cpu {
  uint32_t d[8];
  uint32_t a[8];
  uint32_t pc;
  uint16_t sr;
  uint32_t usp;
  uint32_t ssp;

  bool stopped;
  long cycles;
  uint32_t pending_interrupts;
} m68k_cpu_t;

m68k_cpu_t *m68k_create(void);
void m68k_destroy(m68k_cpu_t *cpu);
void m68k_reset(m68k_cpu_t *cpu);

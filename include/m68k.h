#define SR_C 0x01
#define SR_V 0x02
#define SR_Z 0x04
#define SR_N 0x08
#define SR_X 0x10

typedef struct m68k_cpu m68k_cpu_t;

m68k_cpu_t *m68k_create(void);
void m68k_destroy(m68k_cpu_t *cpu);
void m68k_reset(m68k_cpu_t *cpu);

#ifndef M68K_H
#define M68K_H

#include <stdint.h>

/* Motorola 68000 CPU emulation
 * The NeoGeo uses a 68000 as its main CPU running at 12MHz
 */

typedef struct m68k_cpu m68k_cpu_t;

m68k_cpu_t *m68k_create(void);
void m68k_destroy(m68k_cpu_t *cpu);
void m68k_reset(m68k_cpu_t *cpu);

/* Execute cycles, returns actual cycles executed */
int m68k_execute(m68k_cpu_t *cpu, int cycles);

/* Memory interface callbacks */
typedef uint8_t (*m68k_read8_fn)(void *ctx, uint32_t addr);
typedef uint16_t (*m68k_read16_fn)(void *ctx, uint32_t addr);
typedef void (*m68k_write8_fn)(void *ctx, uint32_t addr, uint8_t val);
typedef void (*m68k_write16_fn)(void *ctx, uint32_t addr, uint16_t val);

void m68k_set_read8(m68k_cpu_t *cpu, m68k_read8_fn fn, void *ctx);
void m68k_set_read16(m68k_cpu_t *cpu, m68k_read16_fn fn, void *ctx);
void m68k_set_write8(m68k_cpu_t *cpu, m68k_write8_fn fn, void *ctx);
void m68k_set_write16(m68k_cpu_t *cpu, m68k_write16_fn fn, void *ctx);

/* Interrupt handling */
void m68k_set_irq(m68k_cpu_t *cpu, int level);

/* Register access */
uint32_t m68k_get_reg(m68k_cpu_t *cpu, int reg);
void m68k_set_reg(m68k_cpu_t *cpu, int reg, uint32_t val);

/* Register indices */
enum {
    M68K_REG_D0, M68K_REG_D1, M68K_REG_D2, M68K_REG_D3,
    M68K_REG_D4, M68K_REG_D5, M68K_REG_D6, M68K_REG_D7,
    M68K_REG_A0, M68K_REG_A1, M68K_REG_A2, M68K_REG_A3,
    M68K_REG_A4, M68K_REG_A5, M68K_REG_A6, M68K_REG_A7,
    M68K_REG_PC, M68K_REG_SR, M68K_REG_USP, M68K_REG_SSP
};

#endif /* M68K_H */

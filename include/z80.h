#ifndef Z80_H
#define Z80_H

#include <stdint.h>

/* Zilog Z80 CPU emulation
 * The NeoGeo uses a Z80 as its sound CPU running at 4MHz
 */

typedef struct z80_cpu z80_cpu_t;

z80_cpu_t *z80_create(void);
void z80_destroy(z80_cpu_t *cpu);
void z80_reset(z80_cpu_t *cpu);

/* Execute cycles, returns actual cycles executed */
int z80_execute(z80_cpu_t *cpu, int cycles);

/* Memory interface callbacks */
typedef uint8_t (*z80_read_fn)(void *ctx, uint16_t addr);
typedef void (*z80_write_fn)(void *ctx, uint16_t addr, uint8_t val);
typedef uint8_t (*z80_io_read_fn)(void *ctx, uint16_t port);
typedef void (*z80_io_write_fn)(void *ctx, uint16_t port, uint8_t val);

void z80_set_read(z80_cpu_t *cpu, z80_read_fn fn, void *ctx);
void z80_set_write(z80_cpu_t *cpu, z80_write_fn fn, void *ctx);
void z80_set_io_read(z80_cpu_t *cpu, z80_io_read_fn fn, void *ctx);
void z80_set_io_write(z80_cpu_t *cpu, z80_io_write_fn fn, void *ctx);

/* Interrupt handling */
void z80_nmi(z80_cpu_t *cpu);
void z80_irq(z80_cpu_t *cpu, uint8_t vector);

#endif /* Z80_H */

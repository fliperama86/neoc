#include <stdlib.h>
#include <string.h>

#include "z80.h"

struct z80_cpu {
    /* Main registers */
    uint8_t a, f;
    uint8_t b, c, d, e, h, l;

    /* Alternate registers */
    uint8_t a_, f_;
    uint8_t b_, c_, d_, e_, h_, l_;

    /* Index registers */
    uint16_t ix, iy;

    /* Stack pointer and program counter */
    uint16_t sp, pc;

    /* Interrupt and refresh registers */
    uint8_t i, r;

    /* Interrupt flip-flops and mode */
    uint8_t iff1, iff2;
    uint8_t im;

    /* Memory callbacks */
    z80_read_fn read;
    z80_write_fn write;
    z80_io_read_fn io_read;
    z80_io_write_fn io_write;
    void *mem_ctx;

    /* State */
    int halted;
    int pending_cycles;
};

z80_cpu_t *z80_create(void) {
    z80_cpu_t *cpu = calloc(1, sizeof(z80_cpu_t));
    return cpu;
}

void z80_destroy(z80_cpu_t *cpu) {
    free(cpu);
}

void z80_reset(z80_cpu_t *cpu) {
    if (!cpu) return;

    cpu->a = cpu->f = 0xFF;
    cpu->b = cpu->c = cpu->d = cpu->e = cpu->h = cpu->l = 0;
    cpu->a_ = cpu->f_ = cpu->b_ = cpu->c_ = 0;
    cpu->d_ = cpu->e_ = cpu->h_ = cpu->l_ = 0;

    cpu->ix = cpu->iy = 0xFFFF;
    cpu->sp = 0xFFFF;
    cpu->pc = 0x0000;

    cpu->i = cpu->r = 0;
    cpu->iff1 = cpu->iff2 = 0;
    cpu->im = 0;

    cpu->halted = 0;
    cpu->pending_cycles = 0;
}

int z80_execute(z80_cpu_t *cpu, int cycles) {
    if (!cpu || !cpu->read) return 0;

    int executed = 0;

    while (executed < cycles) {
        if (cpu->halted) {
            executed += 4;
            continue;
        }

        /* Fetch opcode */
        uint8_t opcode = cpu->read(cpu->mem_ctx, cpu->pc++);

        /* TODO: Implement full Z80 instruction set */
        /* This is a placeholder that consumes cycles */
        executed += 4;

        (void)opcode;
    }

    return executed;
}

void z80_set_read(z80_cpu_t *cpu, z80_read_fn fn, void *ctx) {
    if (cpu) {
        cpu->read = fn;
        cpu->mem_ctx = ctx;
    }
}

void z80_set_write(z80_cpu_t *cpu, z80_write_fn fn, void *ctx) {
    if (cpu) {
        cpu->write = fn;
        cpu->mem_ctx = ctx;
    }
}

void z80_set_io_read(z80_cpu_t *cpu, z80_io_read_fn fn, void *ctx) {
    if (cpu) {
        cpu->io_read = fn;
        cpu->mem_ctx = ctx;
    }
}

void z80_set_io_write(z80_cpu_t *cpu, z80_io_write_fn fn, void *ctx) {
    if (cpu) {
        cpu->io_write = fn;
        cpu->mem_ctx = ctx;
    }
}

void z80_nmi(z80_cpu_t *cpu) {
    if (!cpu) return;

    cpu->halted = 0;
    cpu->iff1 = 0;

    /* Push PC and jump to NMI vector */
    if (cpu->write) {
        cpu->sp--;
        cpu->write(cpu->mem_ctx, cpu->sp, cpu->pc >> 8);
        cpu->sp--;
        cpu->write(cpu->mem_ctx, cpu->sp, cpu->pc & 0xFF);
    }
    cpu->pc = 0x0066;
}

void z80_irq(z80_cpu_t *cpu, uint8_t vector) {
    if (!cpu || !cpu->iff1) return;

    cpu->halted = 0;
    cpu->iff1 = cpu->iff2 = 0;

    if (cpu->write) {
        cpu->sp--;
        cpu->write(cpu->mem_ctx, cpu->sp, cpu->pc >> 8);
        cpu->sp--;
        cpu->write(cpu->mem_ctx, cpu->sp, cpu->pc & 0xFF);
    }

    switch (cpu->im) {
        case 0:
            /* Mode 0: Execute instruction on bus */
            cpu->pc = vector;
            break;
        case 1:
            /* Mode 1: Jump to 0x0038 */
            cpu->pc = 0x0038;
            break;
        case 2:
            /* Mode 2: Vectored interrupt */
            if (cpu->read) {
                uint16_t addr = (cpu->i << 8) | vector;
                cpu->pc = cpu->read(cpu->mem_ctx, addr) |
                         (cpu->read(cpu->mem_ctx, addr + 1) << 8);
            }
            break;
    }
}

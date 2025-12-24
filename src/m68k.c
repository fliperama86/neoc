#include <stdlib.h>
#include <string.h>

#include "m68k.h"

struct m68k_cpu {
    /* Data registers */
    uint32_t d[8];
    /* Address registers */
    uint32_t a[8];
    /* Program counter */
    uint32_t pc;
    /* Status register */
    uint16_t sr;
    /* Stack pointers (USP/SSP) */
    uint32_t usp;
    uint32_t ssp;

    /* Memory callbacks */
    m68k_read8_fn read8;
    m68k_read16_fn read16;
    m68k_write8_fn write8;
    m68k_write16_fn write16;
    void *mem_ctx;

    /* Interrupt state */
    int irq_level;
    int pending_cycles;
};

m68k_cpu_t *m68k_create(void) {
    m68k_cpu_t *cpu = calloc(1, sizeof(m68k_cpu_t));
    return cpu;
}

void m68k_destroy(m68k_cpu_t *cpu) {
    free(cpu);
}

void m68k_reset(m68k_cpu_t *cpu) {
    if (!cpu) return;

    memset(cpu->d, 0, sizeof(cpu->d));
    memset(cpu->a, 0, sizeof(cpu->a));

    /* Initial stack pointer from vector 0 */
    if (cpu->read16) {
        cpu->ssp = (cpu->read16(cpu->mem_ctx, 0x000000) << 16) |
                    cpu->read16(cpu->mem_ctx, 0x000002);
        /* Initial program counter from vector 1 */
        cpu->pc = (cpu->read16(cpu->mem_ctx, 0x000004) << 16) |
                   cpu->read16(cpu->mem_ctx, 0x000006);
    }

    cpu->sr = 0x2700; /* Supervisor mode, interrupts masked */
    cpu->a[7] = cpu->ssp;
    cpu->irq_level = 0;
    cpu->pending_cycles = 0;
}

int m68k_execute(m68k_cpu_t *cpu, int cycles) {
    if (!cpu || !cpu->read16) return 0;

    int executed = 0;

    while (executed < cycles) {
        /* Fetch opcode */
        uint16_t opcode = cpu->read16(cpu->mem_ctx, cpu->pc);
        cpu->pc += 2;

        /* TODO: Implement full 68000 instruction set */
        /* This is a placeholder that consumes cycles */
        executed += 4;

        /* Check for interrupts */
        if (cpu->irq_level > ((cpu->sr >> 8) & 7)) {
            /* TODO: Process interrupt */
            cpu->irq_level = 0;
        }

        (void)opcode;
    }

    return executed;
}

void m68k_set_read8(m68k_cpu_t *cpu, m68k_read8_fn fn, void *ctx) {
    if (cpu) {
        cpu->read8 = fn;
        cpu->mem_ctx = ctx;
    }
}

void m68k_set_read16(m68k_cpu_t *cpu, m68k_read16_fn fn, void *ctx) {
    if (cpu) {
        cpu->read16 = fn;
        cpu->mem_ctx = ctx;
    }
}

void m68k_set_write8(m68k_cpu_t *cpu, m68k_write8_fn fn, void *ctx) {
    if (cpu) {
        cpu->write8 = fn;
        cpu->mem_ctx = ctx;
    }
}

void m68k_set_write16(m68k_cpu_t *cpu, m68k_write16_fn fn, void *ctx) {
    if (cpu) {
        cpu->write16 = fn;
        cpu->mem_ctx = ctx;
    }
}

void m68k_set_irq(m68k_cpu_t *cpu, int level) {
    if (cpu) {
        cpu->irq_level = level & 7;
    }
}

uint32_t m68k_get_reg(m68k_cpu_t *cpu, int reg) {
    if (!cpu) return 0;

    if (reg >= M68K_REG_D0 && reg <= M68K_REG_D7) {
        return cpu->d[reg - M68K_REG_D0];
    } else if (reg >= M68K_REG_A0 && reg <= M68K_REG_A7) {
        return cpu->a[reg - M68K_REG_A0];
    } else if (reg == M68K_REG_PC) {
        return cpu->pc;
    } else if (reg == M68K_REG_SR) {
        return cpu->sr;
    } else if (reg == M68K_REG_USP) {
        return cpu->usp;
    } else if (reg == M68K_REG_SSP) {
        return cpu->ssp;
    }

    return 0;
}

void m68k_set_reg(m68k_cpu_t *cpu, int reg, uint32_t val) {
    if (!cpu) return;

    if (reg >= M68K_REG_D0 && reg <= M68K_REG_D7) {
        cpu->d[reg - M68K_REG_D0] = val;
    } else if (reg >= M68K_REG_A0 && reg <= M68K_REG_A7) {
        cpu->a[reg - M68K_REG_A0] = val;
    } else if (reg == M68K_REG_PC) {
        cpu->pc = val;
    } else if (reg == M68K_REG_SR) {
        cpu->sr = val & 0xFFFF;
    } else if (reg == M68K_REG_USP) {
        cpu->usp = val;
    } else if (reg == M68K_REG_SSP) {
        cpu->ssp = val;
    }
}

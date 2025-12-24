#include <stdlib.h>
#include <string.h>

#include "memory.h"

#define WORK_RAM_SIZE   0x10000     /* 64KB */
#define BIOS_SIZE       0x20000     /* 128KB */
#define SRAM_SIZE       0x10000     /* 64KB */
#define MAX_PROM_SIZE   0x600000    /* 6MB max P-ROM */

struct neoc_memory {
    /* Work RAM (0x100000-0x10FFFF) */
    uint8_t work_ram[WORK_RAM_SIZE];

    /* BIOS ROM (0xC00000-0xC1FFFF) */
    uint8_t *bios;
    uint32_t bios_size;

    /* SRAM (0xD00000-0xD0FFFF) */
    uint8_t sram[SRAM_SIZE];

    /* Program ROM */
    uint8_t *prom;
    uint32_t prom_size;
    int prom_bank;

    /* Memory card area - not implemented yet */
};

neoc_memory_t *memory_create(void) {
    neoc_memory_t *mem = calloc(1, sizeof(neoc_memory_t));
    return mem;
}

void memory_destroy(neoc_memory_t *mem) {
    if (!mem) return;

    free(mem->bios);
    free(mem->prom);
    free(mem);
}

void memory_reset(neoc_memory_t *mem) {
    if (!mem) return;

    memset(mem->work_ram, 0, sizeof(mem->work_ram));
    mem->prom_bank = 0;
}

uint8_t memory_read8(neoc_memory_t *mem, uint32_t addr) {
    if (!mem) return 0xFF;

    addr &= 0xFFFFFF; /* 24-bit address bus */

    if (addr < 0x100000) {
        /* P-ROM area */
        if (mem->prom && addr < mem->prom_size) {
            return mem->prom[addr];
        }
    } else if (addr >= 0x100000 && addr < 0x110000) {
        /* Work RAM */
        return mem->work_ram[addr & 0xFFFF];
    } else if (addr >= 0x200000 && addr < 0x300000) {
        /* Banked P-ROM */
        uint32_t bank_addr = (mem->prom_bank * 0x100000) + (addr & 0xFFFFF);
        if (mem->prom && bank_addr < mem->prom_size) {
            return mem->prom[bank_addr];
        }
    } else if (addr >= 0xC00000 && addr < 0xC20000) {
        /* BIOS */
        if (mem->bios && (addr & 0x1FFFF) < mem->bios_size) {
            return mem->bios[addr & 0x1FFFF];
        }
    } else if (addr >= 0xD00000 && addr < 0xD10000) {
        /* SRAM */
        return mem->sram[addr & 0xFFFF];
    }

    return 0xFF;
}

uint16_t memory_read16(neoc_memory_t *mem, uint32_t addr) {
    uint8_t hi = memory_read8(mem, addr);
    uint8_t lo = memory_read8(mem, addr + 1);
    return (hi << 8) | lo;
}

void memory_write8(neoc_memory_t *mem, uint32_t addr, uint8_t val) {
    if (!mem) return;

    addr &= 0xFFFFFF;

    if (addr >= 0x100000 && addr < 0x110000) {
        /* Work RAM */
        mem->work_ram[addr & 0xFFFF] = val;
    } else if (addr >= 0xD00000 && addr < 0xD10000) {
        /* SRAM */
        mem->sram[addr & 0xFFFF] = val;
    }
    /* ROM areas are read-only */
}

void memory_write16(neoc_memory_t *mem, uint32_t addr, uint16_t val) {
    memory_write8(mem, addr, val >> 8);
    memory_write8(mem, addr + 1, val & 0xFF);
}

int memory_load_program_rom(neoc_memory_t *mem, const uint8_t *data, uint32_t size) {
    if (!mem || !data || size == 0) return -1;

    free(mem->prom);
    mem->prom = malloc(size);
    if (!mem->prom) return -1;

    memcpy(mem->prom, data, size);
    mem->prom_size = size;
    return 0;
}

int memory_load_bios(neoc_memory_t *mem, const uint8_t *data, uint32_t size) {
    if (!mem || !data || size == 0 || size > BIOS_SIZE) return -1;

    free(mem->bios);
    mem->bios = malloc(size);
    if (!mem->bios) return -1;

    memcpy(mem->bios, data, size);
    mem->bios_size = size;
    return 0;
}

void memory_set_program_bank(neoc_memory_t *mem, int bank) {
    if (mem) {
        mem->prom_bank = bank;
    }
}

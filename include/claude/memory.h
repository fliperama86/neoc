#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

/* NeoGeo memory map (main 68000 CPU)
 *
 * 0x000000-0x0FFFFF: Program ROM (1MB, banked for larger games)
 * 0x100000-0x10FFFF: Work RAM (64KB)
 * 0x200000-0x2FFFFF: Program ROM bank area
 * 0x300000-0x3FFFFF: I/O ports
 * 0x400000-0x7FFFFF: Palette, video registers
 * 0x800000-0x8FFFFF: Memory card
 * 0xC00000-0xCFFFFF: BIOS ROM
 * 0xD00000-0xDFFFFF: SRAM (battery backed)
 */

typedef struct neoc_memory neoc_memory_t;

neoc_memory_t *memory_create(void);
void memory_destroy(neoc_memory_t *mem);
void memory_reset(neoc_memory_t *mem);

/* 68000 memory interface */
uint8_t memory_read8(neoc_memory_t *mem, uint32_t addr);
uint16_t memory_read16(neoc_memory_t *mem, uint32_t addr);
void memory_write8(neoc_memory_t *mem, uint32_t addr, uint8_t val);
void memory_write16(neoc_memory_t *mem, uint32_t addr, uint16_t val);

/* ROM loading */
int memory_load_program_rom(neoc_memory_t *mem, const uint8_t *data, uint32_t size);
int memory_load_bios(neoc_memory_t *mem, const uint8_t *data, uint32_t size);

/* Bank switching */
void memory_set_program_bank(neoc_memory_t *mem, int bank);

#endif /* MEMORY_H */

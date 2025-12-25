#include <stdbool.h>
#include <stdint.h>

#define WORK_RAM_SIZE 0x10000 /* 64KB */

typedef struct bus {
  uint8_t ram[WORK_RAM_SIZE];
  uint8_t *prom;
} bus_t;

uint8_t read8(bus_t *bus, uint32_t addr);
void write8(bus_t *bus, uint32_t addr, uint8_t data);

uint16_t read16(bus_t *bus, uint32_t addr);
void write16(bus_t *bus, uint32_t addr, uint16_t data);

uint32_t read32(bus_t *bus, uint32_t addr);
void write32(bus_t *bus, uint32_t addr, uint32_t data);

bus_t *bus_create(void);
void bus_destroy(bus_t *bus);
void bus_reset(bus_t *bus);

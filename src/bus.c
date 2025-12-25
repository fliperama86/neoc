#include "bus.h"
#include <stdlib.h>
#include <string.h>

uint8_t read8(bus_t *bus, uint32_t addr) { return bus->ram[addr]; }

void write8(bus_t *bus, uint32_t addr, uint8_t data) { bus->ram[addr] = data; }

bus_t *bus_create(void) {
  bus_t *bus = calloc(1, sizeof(bus_t));
  bus_reset(bus);

  return bus;
}

void bus_destroy(bus_t *bus) { free(bus); }

void bus_reset(bus_t *bus) {
  if (!bus)
    return;

  memset(bus->ram, 0, WORK_RAM_SIZE);
  bus->prom = 0;
}

uint16_t read16(bus_t *bus, uint32_t addr) {
  uint8_t hi = read8(bus, addr);
  uint8_t lo = read8(bus, addr + 1);
  return (uint16_t)((hi << 8) | lo);
}

uint32_t read32(bus_t *bus, uint32_t addr) {
  uint32_t hi = read16(bus, addr);
  uint32_t lo = read16(bus, addr + 2);
  return (hi << 16) | lo;
}

void write16(bus_t *bus, uint32_t addr, uint16_t data) {
  write8(bus, addr, (uint8_t)(data >> 8));
  write8(bus, addr + 1, (uint8_t)(data & 0xFF));
}

void write32(bus_t *bus, uint32_t addr, uint32_t data) {
  write16(bus, addr, (uint16_t)(data >> 16));
  write16(bus, addr + 2, (uint16_t)(data & 0xFFFF));
}

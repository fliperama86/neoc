#include "bus.h"

uint8_t read8(uint32_t addr) { return 0; }

void write8(uint32_t addr, uint8_t data) {
  (void)addr;
  (void)data;
}

// Combine two bytes for a 16-bit read (Big Endian)
uint16_t read16(uint32_t addr) {
  uint8_t hi = read8(addr);
  uint8_t lo = read8(addr + 1);
  return (uint16_t)((hi << 8) | lo);
}

uint32_t read32(uint32_t addr) {
  uint32_t hi = read16(addr);
  uint32_t lo = read16(addr + 2);
  return (hi << 16) | lo;
}

void write16(uint32_t addr, uint16_t data) {
  write8(addr, (uint8_t)(data >> 8));
  write8(addr + 1, (uint8_t)(data & 0xFF));
}

void write32(uint32_t addr, uint32_t data) {
  write16(addr, (uint16_t)(data >> 16));
  write16(addr + 2, (uint16_t)(data & 0xFFFF));
}

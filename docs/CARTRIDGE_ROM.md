# Neo Geo Cartridge & ROM Format Specification

This document describes the Neo Geo cartridge ROM structure, file formats,
and how to load them for emulation.

---

## Overview

Neo Geo cartridges contain multiple ROM chips organized on two boards:

| Board | Contains | Accessed By |
|-------|----------|-------------|
| **PROG** | P ROM, V ROM | 68000 (P), YM2610 (V) |
| **CHA** | C ROM, S ROM, M ROM | LSPC (C, S), Z80 (M) |

---

## ROM Types

### P ROM (Program)

| Property | Value |
|----------|-------|
| CPU | 68000 |
| Data width | 16-bit |
| Max size (no banking) | 1 MB |
| Max size (with banking) | 6+ MB |
| Endianness | Big-endian (68000 native) |
| File extension | `.p1`, `.p2`, `.ep1`, `.sp1` |

**Memory Mapping:**

| P ROM Size | $000000-$0FFFFF | $200000-$2FFFFF |
|------------|-----------------|-----------------|
| ≤ 1 MB | P ROM | (mirror or unmapped) |
| 2 MB | P ROM second half | P ROM first half |
| > 2 MB | First 1 MB | Bankswitched |

**Bankswitching:**
- Write to `$200000-$2FFFFF` to select bank
- Bank number in lower bits of written value
- 1 MB banks mapped into `$200000-$2FFFFF`

**Byte Order:**
P ROMs are stored in native 68000 big-endian format. MAME loads with `load16_word_swap`,
meaning the ROM file bytes may be swapped depending on source.

---

### C ROM (Character/Sprites)

| Property | Value |
|----------|-------|
| System | LSPC graphics chip |
| Data width | 16-bit (paired ROMs) |
| Tile size | 16x16 pixels, 4bpp |
| Bytes per tile | 128 bytes |
| File extensions | `.c1`, `.c2`, `.c3`, `.c4`, etc. |

**ROM Pairing:**
C ROMs come in pairs:
- **Odd ROMs** (C1, C3, C5...): Bitplanes 0 and 1
- **Even ROMs** (C2, C4, C6...): Bitplanes 2 and 3

**Interleaving:**
To decode, interleave C1 and C2 in 2-byte units:
```
Output[0..1] = C1[0..1]
Output[2..3] = C2[0..1]
Output[4..5] = C1[2..3]
Output[6..7] = C2[2..3]
...
```

**Tile Layout:**
Each 16x16 tile is stored as four 8x8 blocks in this order:
```
+---+---+
| 2 | 0 |   Block order in ROM:
+---+---+   0: top-right (x=8, y=0)
| 3 | 1 |   1: bottom-right (x=8, y=8)
+---+---+   2: top-left (x=0, y=0)
            3: bottom-left (x=0, y=8)
```

**Pixel Format (per 8x8 block):**
```
For each row (8 rows per block):
  - 2 bytes from C1 (bitplanes 0, 1)
  - 2 bytes from C2 (bitplanes 2, 3)
  = 4 bytes per row, 32 bytes per 8x8 block
  = 128 bytes per 16x16 tile
```

**Decoding Pseudocode:**
```c
// For each pixel in an 8x8 block:
for (int y = 0; y < 8; y++) {
    uint16_t c1_word = read16(c1_rom, tile_offset + y * 2);
    uint16_t c2_word = read16(c2_rom, tile_offset + y * 2);

    for (int x = 0; x < 8; x++) {
        int bit = 7 - x;  // Pixels stored right-to-left
        int pixel = ((c1_word >> bit) & 1)         // bit 0
                  | ((c1_word >> (bit + 7)) & 2)   // bit 1
                  | ((c2_word >> (bit - 1)) & 4)   // bit 2
                  | ((c2_word >> (bit + 6)) & 8);  // bit 3
        // pixel is 0-15 (0 = transparent)
    }
}
```

---

### S ROM (Fix Layer)

| Property | Value |
|----------|-------|
| System | LSPC (fix layer) |
| Data width | 8-bit |
| Tile size | 8x8 pixels, 4bpp |
| Bytes per tile | 32 bytes |
| Max size | 128 KB |
| File extension | `.s1` |

**Purpose:**
Non-scrolling overlay layer for HUD, scores, text. Always on top of sprites.

**Tile Format:**
```
Each 8x8 tile = 32 bytes
  - 4 bitplanes
  - 8 bytes per bitplane
  - Pixels stored 8 per byte (1bpp per plane)
```

---

### M ROM (Music/Z80 Program)

| Property | Value |
|----------|-------|
| CPU | Z80 |
| Data width | 8-bit |
| Max size | 64 KB (banked: 512 KB+) |
| File extension | `.m1` |

**Memory Mapping:**
- Z80 address space: 64 KB
- First 32 KB: Fixed (start of M ROM)
- Upper 32 KB: Bankswitched

---

### V ROM (Voice/ADPCM Samples)

| Property | Value |
|----------|-------|
| System | YM2610 ADPCM |
| Data width | 8-bit |
| File extensions | `.v1`, `.v2`, `.v11`, `.v21`, etc. |

**Organization:**
- ADPCM-A samples: Fixed addressing
- ADPCM-B samples: Bankswitched

V ROMs are accessed directly by the YM2610, not the CPUs.

---

## MAME Romset Structure

MAME uses ZIP files with standardized naming:

```
gamename.zip
├── 000-p1.p1       # P ROM (program)
├── 000-s1.s1       # S ROM (fix layer)
├── 000-m1.m1       # M ROM (Z80 program)
├── 000-v1.v1       # V ROM (samples)
├── 000-v2.v2       # V ROM (more samples)
├── 000-c1.c1       # C ROM odd (sprites)
├── 000-c2.c2       # C ROM even (sprites)
├── 000-c3.c3       # C ROM odd (more sprites)
└── 000-c4.c4       # C ROM even (more sprites)
```

The `000` prefix varies by game. Some games use different prefixes:
- `xxx-yy.zz` where `xxx` = game ID, `yy` = chip number, `zz` = type

---

## BIOS (neogeo.zip)

The BIOS is required and stored separately:

```
neogeo.zip
├── sp-s2.sp1           # MVS BIOS (various versions)
├── sp-s.sp1
├── sp-e.sp1
├── neo-epo.sp1         # AES BIOS
├── neo-po.sp1
├── 000-lo.lo           # LO ROM (sprite shrink table)
├── sfix.sfix           # System fix layer tiles
└── sm1.sm1             # System Z80 program
```

**Key BIOS Components:**

| File | Size | Purpose |
|------|------|---------|
| `sp-s2.sp1` | 128 KB | MVS system ROM (BIOS) |
| `000-lo.lo` | 64 KB | Sprite shrink lookup table |
| `sfix.sfix` | 128 KB | System fix layer graphics |
| `sm1.sm1` | 128 KB | System Z80 ROM |

---

## ROM Loading Algorithm

### Step 1: Load BIOS

```c
// Load system ROM
load_file("neogeo.zip", "sp-s2.sp1", bios, 0x20000);

// Load LO ROM (sprite shrink table)
load_file("neogeo.zip", "000-lo.lo", lo_rom, 0x10000);

// Load system fix tiles
load_file("neogeo.zip", "sfix.sfix", sfix_rom, 0x20000);

// Load system Z80 ROM
load_file("neogeo.zip", "sm1.sm1", sm1_rom, 0x20000);
```

### Step 2: Load Game ROMs

```c
// P ROM - load and handle byte order
load_p_rom("game.zip", "xxx-p1.p1", p_rom, &p_rom_size);

// S ROM
load_file("game.zip", "xxx-s1.s1", s_rom, 0x20000);

// M ROM
load_file("game.zip", "xxx-m1.m1", m_rom, 0x10000);

// V ROMs - concatenate if multiple
load_v_roms("game.zip", v_rom, &v_rom_size);

// C ROMs - load pairs and interleave
load_c_roms("game.zip", c_rom, &c_rom_size);
```

### Step 3: C ROM Interleaving

```c
void interleave_c_roms(uint8_t *c1, uint8_t *c2,
                       uint8_t *output, size_t size) {
    for (size_t i = 0; i < size; i += 2) {
        output[i * 2 + 0] = c1[i + 0];
        output[i * 2 + 1] = c1[i + 1];
        output[i * 2 + 2] = c2[i + 0];
        output[i * 2 + 3] = c2[i + 1];
    }
}
```

---

## Game Header

The P ROM contains a header at offset `$000100`:

| Offset | Size | Description |
|--------|------|-------------|
| $100 | 2 | Game ID |
| $102 | 2 | Logo tile number |
| $104 | 32 | Game title (ASCII) |
| $124 | 4 | Reserved |
| $128 | 4 | Pointer to eye-catcher routine |
| $12C | 4 | Pointer to title routine |
| $130 | 4 | Pointer to game entry point |
| ... | ... | More vectors |

**NGH (Neo Geo Header) Number:**
Located at `$0108`, identifies the game for memory card saves.

---

## Encryption (Later Games)

Some later games encrypt ROM data:

### C ROM Encryption
Games like KOF99, Garou encrypt C ROM data. Decryption requires:
- Game-specific lookup tables
- XOR operations on tile data

### P ROM Encryption (SMA)
Some games use SMA (Special Memory Address) encryption:
- Bankswitch address shuffling
- Opcode modifications

For early emulator development, start with unencrypted games.

---

## Recommended Test Games (Unencrypted)

| Game | Size | Notes |
|------|------|-------|
| NAM-1975 | Small | Launch title, simple |
| Puzzled | Small | Puzzle game, basic |
| League Bowling | Small | Simple graphics |
| Blue's Journey | Medium | Platform game |
| Fatal Fury | Medium | Fighting game |

---

## References

- [NeoGeo Dev Wiki - P ROM](https://wiki.neogeodev.org/index.php?title=P_ROM)
- [NeoGeo Dev Wiki - C ROM](https://wiki.neogeodev.org/index.php?title=C_ROM)
- [NeoGeo Dev Wiki - Sprite Graphics Format](https://wiki.neogeodev.org/index.php?title=Sprite_graphics_format)
- [NeoGeo Dev Wiki - Bankswitching](https://wiki.neogeodev.org/index.php?title=Bankswitching)
- [NeoGeo Dev Wiki - 68k Memory Map](https://wiki.neogeodev.org/index.php?title=68k_memory_map)
- [MAME Source - neogeo.xml](https://github.com/mamedev/mame/blob/master/hash/neogeo.xml)

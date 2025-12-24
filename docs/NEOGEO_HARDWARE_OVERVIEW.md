# Neo Geo Hardware Overview

This document provides a high-level overview of the Neo Geo hardware architecture.
For detailed component specifications, see the individual documentation files.

---

## System Variants

The Neo Geo platform exists in two primary hardware variants:

| Variant | Name | Target |
|---------|------|--------|
| **MVS** | Multi Video System | Arcade cabinets (1-6 slots) |
| **AES** | Advanced Entertainment System | Home console |

Both variants share identical core hardware and can execute the same machine code.
The primary differences are:
- Cartridge pinout (to prevent arcade operators from using cheaper home carts)
- Physical form factor
- MVS has coin slots, dipswitch settings, and multi-cart support

---

## Hardware Specifications Summary

### CPUs

| Component | Chip | Clock | Bus Width | Role |
|-----------|------|-------|-----------|------|
| Main CPU | Motorola 68000 | 12 MHz | 16/32-bit | Game logic, main program execution |
| Sound CPU | Zilog Z80 | 4 MHz | 8-bit | Sound/music control, YM2610 interface |

The system was marketed as "24-bit" (16 + 8), though it is technically a 16-bit system
with an 8-bit coprocessor.

### Memory

| Type | Size | Description |
|------|------|-------------|
| Work RAM | 64 KB | Shared: 32 KB for BIOS, 32 KB for game |
| Video RAM (Slow) | 64 KB | Sprite/tile data |
| Video RAM (Fast) | 4 KB | Active sprite list, quick access |
| Palette RAM | 16 KB | Color palettes (8 KB accessible) |
| Z80 RAM | 2 KB | Sound CPU work memory |

### Video

| Specification | Value |
|---------------|-------|
| Resolution | 320x224 (NTSC), 320x256 (PAL) |
| Active area | 304x224 (common, 8-pixel borders) |
| Color depth | 16-bit (RGB666 + shared bit) |
| Palette size | 65,536 colors |
| On-screen colors | 4,096 maximum |
| Sprite count | 381 maximum on-screen |
| Sprites per scanline | 96 maximum |
| Sprite size | 16 pixels wide, 2-512 pixels tall |
| Tile size | 16x16 pixels, 15 colors + transparency |
| Background layers | 0 (sprites only) |
| Fix layer | 1 (non-scrolling overlay for HUD/text) |

### Audio

| Component | Specification |
|-----------|---------------|
| Sound chip | Yamaha YM2610 (OPNB) |
| DAC | Yamaha YM3016 (16-bit) |
| FM channels | 4 (4 operators each) |
| SSG channels | 3 (AY-3-8910 compatible) |
| ADPCM-A | 6 channels @ 18.5 kHz fixed rate |
| ADPCM-B | 1 channel @ 1.85-55.5 kHz variable rate |
| Total channels | 15 (4 FM + 3 SSG + 1 noise + 7 ADPCM) |

---

## System Architecture

```
                    +------------------+
                    |   Cartridge      |
                    |  +------------+  |
                    |  | P ROM      |  |  Program (68000 code)
                    |  | S ROM      |  |  Fix layer tiles
                    |  | C ROM      |  |  Sprite graphics
                    |  | M ROM      |  |  Z80 program
                    |  | V ROM      |  |  Audio samples
                    |  +------------+  |
                    +--------+---------+
                             |
         +-------------------+-------------------+
         |                   |                   |
+--------v--------+  +-------v--------+  +-------v-------+
|  Main CPU       |  | Graphics       |  | Sound         |
|  MC68000 @12MHz |  | LSPC + NEO-B1  |  | Z80 @4MHz     |
|                 |  |                |  | YM2610        |
+--------+--------+  +-------+--------+  +-------+-------+
         |                   |                   |
         v                   v                   v
+------------------+  +-------------+     +-----------+
| 64KB Work RAM    |  | 68KB VRAM   |     | 2KB RAM   |
| (32KB BIOS +     |  | 16KB Palette|     |           |
|  32KB Game)      |  +-------------+     +-----------+
+------------------+         |
                             v
                    +------------------+
                    |  Video Output    |
                    |  320x224 @ 60Hz  |
                    +------------------+
```

---

## Cartridge ROM Types

Neo Geo cartridges are split into two boards: **PROG** and **CHA**.

### PROG Board

| ROM | Name | Width | Description |
|-----|------|-------|-------------|
| **P ROM** | Program | 16-bit | 68000 executable code |
| **V ROM** | Voice | 8-bit | ADPCM audio samples |

### CHA Board

| ROM | Name | Width | Description |
|-----|------|-------|-------------|
| **C ROM** | Character | 16-bit (pairs) | Sprite graphics data |
| **S ROM** | Fix/Static | 8-bit | Fix layer tile graphics |
| **M ROM** | Music | 8-bit | Z80 sound program |

Original max capacity: 330 Mbit. Later games used bank switching for ~716 Mbit.

---

## Memory Map (68000)

| Address Range | Size | Description |
|---------------|------|-------------|
| `$000000-$0FFFFF` | 1 MB | P ROM (program, bankswitched) |
| `$100000-$10FFFF` | 64 KB | Work RAM |
| `$100000-$107FFF` | 32 KB | Game RAM |
| `$108000-$10FFFF` | 32 KB | BIOS RAM |
| `$200000-$2FFFFF` | 1 MB | P ROM Bank (switchable) |
| `$300000-$3FFFFF` | 1 MB | I/O registers |
| `$400000-$401FFF` | 8 KB | Palette RAM |
| `$800000-$BFFFFF` | 4 MB | Memory card |
| `$C00000-$C1FFFF` | 128 KB | System ROM (BIOS) |
| `$D00000-$DFFFFF` | 1 MB | BIOS ROM (bankswitched) |

Key register ranges in `$300000-$3FFFFF`:
- `$300000`: REG_P1CNT (Player 1 controls)
- `$300001`: REG_DIPSW (DIP switches, active low bits direct accent of cabinet type)
- `$320000`: REG_SOUND (Z80 communication)
- `$380000`: REG_POUTPUT (I/O output)
- `$3A0000+`: REG_SWPROM, REG_SWPBIOS (vector table swap)
- `$3C0000+`: Video registers (LSPC)

---

## Graphics System (LSPC)

The Neo Geo uses a sprite-based graphics system with no hardware background layers.
Backgrounds are composed of large sprite strips.

### Key Chips

| Chip | Role |
|------|------|
| **LSPC** (LSPC-A0/A2) | Line Sprite Controller - generates ROM addresses, coordinates rendering |
| **NEO-B0/B1** | Sprite line buffer, pixel output |
| **NEO-C0** | Address multiplexer (early boards) |

### Rendering Pipeline

1. **Parsing** (line N-2): Check Y positions of all 381 sprites to build active list
2. **Rendering** (line N-1): Fetch tile data from C ROM for visible sprites
3. **Output** (line N): Display from completed line buffer

The system uses double-buffered line buffers (320 pixels each).

### Sprite System

- Sprites are vertical strips: 16 pixels wide, up to 512 pixels tall (32 tiles)
- Each sprite has position, tile map, palette, and flip attributes
- Sprites can be chained together for larger objects
- Hardware sprite shrinking via lookup tables in LO ROM

### Fix Layer

- Non-scrolling 40x32 tile layer (8x8 tiles)
- Always rendered on top of sprites
- Used for HUD, score, text overlays
- Data comes from S ROM

---

## Sound System

### Architecture

```
+----------+     +----------+     +----------+
| 68000    |---->| Z80      |---->| YM2610   |----> Audio Out
| (1 byte) |     | @4MHz    |     | (OPNB)   |
+----------+     +----+-----+     +----+-----+
                      |                |
                 +----v----+      +----v----+
                 | M ROM   |      | V ROM   |
                 | (Z80    |      | (ADPCM  |
                 | program)|      | samples)|
                 +---------+      +---------+
```

### Communication

- 68000 writes a single byte to `$320000` (REG_SOUND)
- Z80 reads from port `$00`, processes command
- Z80 has exclusive access to YM2610

### YM2610 Capabilities

| Channel Type | Count | Description |
|--------------|-------|-------------|
| FM | 4 | 4-operator FM synthesis |
| SSG | 3 | Square wave (PSG compatible) |
| Noise | 1 | White noise generator |
| ADPCM-A | 6 | Fixed 18.5 kHz sample playback |
| ADPCM-B | 1 | Variable rate (1.85-55.5 kHz) |

---

## BIOS / System ROM

The System ROM (BIOS) resides at `$C00000-$C1FFFF` and provides:

### Boot Sequence

1. Hardware initialization
2. Eye-catcher animation (SNK logo)
3. Gameings header validation
4. Vector table swap (BIOS -> Game)
5. Jump to game entry point

### System Calls

BIOS calls are accessed via jump table at `$C00402-$C004D4` (6 bytes per entry).

Key functions:
- Controller input reading
- Memory card access
- VRAM initialization
- Calendar/RTC access (MVS)
-Ings validation

### Vector Table Swap

Two registers control which vector table is active:
- `REG_SWPBIOS` (`$3A0003`): Switch to BIOS vectors
- `REG_SWPROM` (`$3A0001`): Switch to cartridge vectors

On reset, BIOS vectors are active. Before launching game, cartridge vectors are swapped in.

### Reserved RAM

| Address Range | Usage |
|---------------|-------|
| `$10F300-$10FFFF` | BIOS reserved area |
| `$10FD80-$10FDAF` | Controller state |
| `$10FE00+` | System variables |

Games typically set SP (stack pointer) to `$10F300`.

---

## Timing

| Parameter | Value |
|-----------|-------|
| Master clock | 24 MHz |
| 68000 clock | 12 MHz (master / 2) |
| Z80 clock | 4 MHz (master / 6) |
| Pixel clock | 6 MHz |
| Frame rate | 59.185606 Hz (NTSC) |
| Scanlines | 264 per frame |
| H-Blank | ~52 pixels |
| V-Blank | 40 scanlines |

---

## I/O and Controls

### Player Input

- 4-direction joystick + 4 action buttons (A, B, C, D)
- Start button, Select button
- MVS: coin slots, test/service buttons

### Memory Card

- 2 KB to 16 KB battery-backed SRAM
- 64-byte blocks across 5 segments
- Cross-game save data support

### MVS-Specific

- DIP switches for operator settings
- Coin counters and lockout
- Multi-slot game switching
- Soft DIPs (in-game configuration)

---

## References

### Primary Sources
- [NeoGeo Development Wiki](https://wiki.neogeodev.org/index.php?title=Main_Page)
- [Neo-Geo Programming Manual (PDF)](http://furrtek.free.fr/noclass/neogeo/NeoGeoPM.pdf)
- [Neo Geo Architecture - Copetti](https://www.copetti.org/writings/consoles/neogeo/)
- [Neo-Geo Programming Guide](https://www.ajworld.net/neogeodev/neoguide/?p=1)

### Additional Resources
- [NeoGeoBook](https://neogeobook.mattgreer.dev/book/02-the-hardware)
- [RetroSix Wiki - Neo Geo AES](https://www.retrosix.wiki/lspc-line-sprite-processor-neo-geo-aes)
- [68000 Assembly for NeoGeo](https://www.chibiakumas.com/68000/neogeo.php)

---

## Component Documentation (TODO)

Detailed specifications for each component will be documented separately:

- [ ] `CPU_68000.md` - Main CPU detailed specification
- [ ] `CPU_Z80.md` - Sound CPU detailed specification
- [ ] `MEMORY_MAP.md` - Complete memory map with all registers
- [ ] `VIDEO_LSPC.md` - Graphics system and LSPC chip
- [ ] `AUDIO_YM2610.md` - Sound chip and audio subsystem
- [ ] `CARTRIDGE.md` - ROM formats and bank switching
- [ ] `BIOS.md` - System ROM calls and behavior
- [ ] `IO.md` - Input/output and controls
- [ ] `TIMING.md` - Clock domains and synchronization

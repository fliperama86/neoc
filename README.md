# neoc

A Neo Geo emulator written in C.

## Status

Early development - CPU implementation in progress.

## Building

```bash
# macOS / Linux
./build.sh

# Windows
build.bat
```

Or using CMake directly:

```bash
cmake --preset default
cmake --build build
```

## Project Structure

```
neoc/
├── src/           # Source files
│   ├── m68k.c     # Motorola 68000 CPU
│   ├── z80.c      # Zilog Z80 CPU
│   ├── memory.c   # Memory map
│   ├── video.c    # Graphics (LSPC)
│   └── ym2610.c   # Audio
├── include/       # Header files
├── docs/          # Hardware documentation
└── tests/         # Unit tests
```

## Documentation

See `docs/` for hardware specifications:
- `NEOGEO_HARDWARE_OVERVIEW.md` - System overview
- `CPU_68000.md` - Main CPU specification

## License

TBD

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "neoc.h"
#include "m68k.h"
#include "z80.h"
#include "ym2610.h"
#include "video.h"
#include "memory.h"
#include "input.h"

/* NeoGeo timing constants */
#define NEOC_M68K_CLOCK     12000000    /* 12 MHz */
#define NEOC_Z80_CLOCK      4000000     /* 4 MHz */
#define NEOC_YM2610_CLOCK   8000000     /* 8 MHz */
#define NEOC_FPS            59.185606   /* ~59.19 Hz */
#define NEOC_SCANLINES      264
#define NEOC_CYCLES_PER_FRAME (NEOC_M68K_CLOCK / 60)

struct neoc {
    m68k_cpu_t *m68k;
    z80_cpu_t *z80;
    ym2610_t *ym2610;
    neoc_video_t *video;
    neoc_memory_t *memory;
    neoc_input_t *input;

    bool running;
    int frame_count;
};

neoc_t *neoc_create(void) {
    neoc_t *emu = calloc(1, sizeof(neoc_t));
    if (!emu) return NULL;

    emu->m68k = m68k_create();
    emu->z80 = z80_create();
    emu->ym2610 = ym2610_create(NEOC_YM2610_CLOCK, 44100);
    emu->video = video_create();
    emu->memory = memory_create();
    emu->input = input_create();

    if (!emu->m68k || !emu->z80 || !emu->ym2610 ||
        !emu->video || !emu->memory || !emu->input) {
        neoc_destroy(emu);
        return NULL;
    }

    neoc_reset(emu);
    return emu;
}

void neoc_destroy(neoc_t *emu) {
    if (!emu) return;

    m68k_destroy(emu->m68k);
    z80_destroy(emu->z80);
    ym2610_destroy(emu->ym2610);
    video_destroy(emu->video);
    memory_destroy(emu->memory);
    input_destroy(emu->input);

    free(emu);
}

void neoc_reset(neoc_t *emu) {
    if (!emu) return;

    m68k_reset(emu->m68k);
    z80_reset(emu->z80);
    ym2610_reset(emu->ym2610);
    video_reset(emu->video);
    memory_reset(emu->memory);
    input_reset(emu->input);

    emu->running = true;
    emu->frame_count = 0;
}

int neoc_load_rom(neoc_t *emu, const char *path) {
    if (!emu || !path) return -1;

    /* TODO: Implement ROM loading (ZIP archive or directory) */
    /* NeoGeo ROMs typically contain:
     * - P ROM: Program ROM (68000 code)
     * - S ROM: Fix layer graphics
     * - C ROM: Sprite graphics
     * - M ROM: Z80 program
     * - V ROM: ADPCM samples
     */

    (void)path;
    printf("ROM loading not yet implemented\n");
    return 0;
}

void neoc_run_frame(neoc_t *emu) {
    if (!emu || !emu->running) return;

    int cycles_per_line = NEOC_CYCLES_PER_FRAME / NEOC_SCANLINES;

    for (int line = 0; line < NEOC_SCANLINES; line++) {
        /* Run 68000 for this scanline */
        m68k_execute(emu->m68k, cycles_per_line);

        /* Run Z80 (proportionally fewer cycles) */
        int z80_cycles = (cycles_per_line * NEOC_Z80_CLOCK) / NEOC_M68K_CLOCK;
        z80_execute(emu->z80, z80_cycles);

        /* Render visible scanlines */
        if (line < NEOC_SCREEN_HEIGHT) {
            video_render_scanline(emu->video, line);
        }

        /* VBlank interrupt at line 224 */
        if (line == NEOC_SCREEN_HEIGHT) {
            m68k_set_irq(emu->m68k, 1);
        }
    }

    video_end_frame(emu->video);
    emu->frame_count++;
}

bool neoc_is_running(neoc_t *emu) {
    return emu && emu->running;
}

void neoc_stop(neoc_t *emu) {
    if (emu) {
        emu->running = false;
    }
}

int neoc_save_state(neoc_t *emu, const char *path) {
    if (!emu || !path) return -1;
    /* TODO: Implement state saving */
    (void)path;
    return -1;
}

int neoc_load_state(neoc_t *emu, const char *path) {
    if (!emu || !path) return -1;
    /* TODO: Implement state loading */
    (void)path;
    return -1;
}

neoc_input_t *neoc_get_input(neoc_t *emu) {
    return emu ? emu->input : NULL;
}

const uint32_t *neoc_get_framebuffer(neoc_t *emu) {
    return emu ? video_get_framebuffer(emu->video) : NULL;
}

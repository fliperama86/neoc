#include <stdlib.h>
#include <string.h>

#include "video.h"

/* NeoGeo video specs:
 * - 320x224 visible resolution
 * - 380 sprites total, 96 per scanline
 * - Sprites are composed of 16x16 tiles
 * - Fix layer: 40x32 tiles of 8x8 pixels
 * - 4096 colors per palette, 256 palettes
 */

#define VRAM_SIZE       0x20000
#define PALETTE_SIZE    0x2000
#define MAX_SPRITES     380
#define SPRITES_PER_LINE 96

struct neoc_video {
    /* VRAM */
    uint16_t vram[VRAM_SIZE / 2];

    /* Palette RAM (two banks) */
    uint16_t palette[PALETTE_SIZE / 2];
    int palette_bank;

    /* Sprite control registers */
    uint16_t sprite_shrink[MAX_SPRITES];
    uint16_t sprite_list[MAX_SPRITES];

    /* Video registers */
    uint16_t vram_addr;
    uint16_t vram_mod;
    uint16_t irq_pos;

    /* ROM data */
    const uint8_t *sprite_rom;
    uint32_t sprite_rom_size;
    const uint8_t *fix_rom;
    uint32_t fix_rom_size;

    /* Framebuffer (RGBA8888) */
    uint32_t framebuffer[NEOC_SCREEN_WIDTH * NEOC_SCREEN_HEIGHT];
};

neoc_video_t *video_create(void) {
    neoc_video_t *video = calloc(1, sizeof(neoc_video_t));
    return video;
}

void video_destroy(neoc_video_t *video) {
    free(video);
}

void video_reset(neoc_video_t *video) {
    if (!video) return;

    memset(video->vram, 0, sizeof(video->vram));
    memset(video->palette, 0, sizeof(video->palette));
    memset(video->framebuffer, 0, sizeof(video->framebuffer));

    video->vram_addr = 0;
    video->vram_mod = 1;
    video->palette_bank = 0;
}

void video_write_vram(neoc_video_t *video, uint16_t addr, uint16_t data) {
    if (!video) return;

    addr &= (VRAM_SIZE / 2 - 1);
    video->vram[addr] = data;
}

uint16_t video_read_vram(neoc_video_t *video, uint16_t addr) {
    if (!video) return 0;

    addr &= (VRAM_SIZE / 2 - 1);
    return video->vram[addr];
}

void video_write_reg(neoc_video_t *video, uint8_t reg, uint16_t data) {
    if (!video) return;

    switch (reg) {
        case 0: /* VRAM address */
            video->vram_addr = data;
            break;
        case 1: /* VRAM data */
            video->vram[video->vram_addr & 0xFFFF] = data;
            video->vram_addr += video->vram_mod;
            break;
        case 2: /* VRAM modulo */
            video->vram_mod = data;
            break;
        case 5: /* IRQ position */
            video->irq_pos = data;
            break;
        case 6: /* Palette bank */
            video->palette_bank = data & 1;
            break;
    }
}

uint16_t video_read_reg(neoc_video_t *video, uint8_t reg) {
    if (!video) return 0;

    switch (reg) {
        case 0:
            return video->vram_addr;
        case 1:
            return video->vram[video->vram_addr & 0xFFFF];
    }

    return 0;
}

void video_write_palette(neoc_video_t *video, uint16_t addr, uint16_t data) {
    if (!video) return;

    addr &= (PALETTE_SIZE / 2 - 1);
    video->palette[addr] = data;
}

uint16_t video_read_palette(neoc_video_t *video, uint16_t addr) {
    if (!video) return 0;

    addr &= (PALETTE_SIZE / 2 - 1);
    return video->palette[addr];
}

/* Convert NeoGeo RGB color to RGBA8888 */
static uint32_t color_to_rgba(uint16_t color) {
    /* NeoGeo color format: xBBBBBGGGGGRRRRR (dark bit in MSB) */
    int r = (color & 0x0F) | ((color >> 10) & 0x10);
    int g = ((color >> 4) & 0x0F) | ((color >> 11) & 0x10);
    int b = ((color >> 8) & 0x0F) | ((color >> 12) & 0x10);

    /* Scale 5-bit to 8-bit */
    r = (r << 3) | (r >> 2);
    g = (g << 3) | (g >> 2);
    b = (b << 3) | (b >> 2);

    return (0xFF << 24) | (b << 16) | (g << 8) | r;
}

void video_render_scanline(neoc_video_t *video, int line) {
    if (!video || line < 0 || line >= NEOC_SCREEN_HEIGHT) return;

    uint32_t *dest = &video->framebuffer[line * NEOC_SCREEN_WIDTH];

    /* Clear to backdrop color */
    uint32_t backdrop = color_to_rgba(video->palette[0]);
    for (int x = 0; x < NEOC_SCREEN_WIDTH; x++) {
        dest[x] = backdrop;
    }

    /* TODO: Render sprites */
    /* TODO: Render fix layer */
}

void video_end_frame(neoc_video_t *video) {
    if (!video) return;
    /* Frame complete - framebuffer is ready */
}

const uint32_t *video_get_framebuffer(neoc_video_t *video) {
    if (!video) return NULL;
    return video->framebuffer;
}

void video_set_sprite_rom(neoc_video_t *video, const uint8_t *data, uint32_t size) {
    if (!video) return;
    video->sprite_rom = data;
    video->sprite_rom_size = size;
}

void video_set_fix_rom(neoc_video_t *video, const uint8_t *data, uint32_t size) {
    if (!video) return;
    video->fix_rom = data;
    video->fix_rom_size = size;
}

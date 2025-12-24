#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

/* NeoGeo video system emulation
 * Resolution: 320x224
 * Sprites: 380 sprites, 96 per scanline max
 * Tilemaps: Fix layer (text/HUD) + Sprite layer
 */

#define NEOC_SCREEN_WIDTH  320
#define NEOC_SCREEN_HEIGHT 224

typedef struct neoc_video neoc_video_t;

neoc_video_t *video_create(void);
void video_destroy(neoc_video_t *video);
void video_reset(neoc_video_t *video);

/* VRAM access */
void video_write_vram(neoc_video_t *video, uint16_t addr, uint16_t data);
uint16_t video_read_vram(neoc_video_t *video, uint16_t addr);

/* Register access */
void video_write_reg(neoc_video_t *video, uint8_t reg, uint16_t data);
uint16_t video_read_reg(neoc_video_t *video, uint8_t reg);

/* Palette access (two palette banks) */
void video_write_palette(neoc_video_t *video, uint16_t addr, uint16_t data);
uint16_t video_read_palette(neoc_video_t *video, uint16_t addr);

/* Rendering */
void video_render_scanline(neoc_video_t *video, int line);
void video_end_frame(neoc_video_t *video);

/* Get framebuffer (RGBA8888 format) */
const uint32_t *video_get_framebuffer(neoc_video_t *video);

/* ROM data */
void video_set_sprite_rom(neoc_video_t *video, const uint8_t *data, uint32_t size);
void video_set_fix_rom(neoc_video_t *video, const uint8_t *data, uint32_t size);

#endif /* VIDEO_H */

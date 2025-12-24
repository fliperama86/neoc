#ifndef YM2610_H
#define YM2610_H

#include <stdint.h>

/* Yamaha YM2610 (OPNB) sound chip emulation
 * The NeoGeo's primary sound chip with FM synthesis and ADPCM
 */

typedef struct ym2610 ym2610_t;

ym2610_t *ym2610_create(int clock, int sample_rate);
void ym2610_destroy(ym2610_t *chip);
void ym2610_reset(ym2610_t *chip);

/* Register access */
void ym2610_write(ym2610_t *chip, uint8_t addr, uint8_t data);
uint8_t ym2610_read(ym2610_t *chip, uint8_t addr);

/* Audio generation - fills stereo interleaved buffer */
void ym2610_update(ym2610_t *chip, int16_t *buffer, int samples);

/* ADPCM ROM access */
void ym2610_set_adpcm_a_rom(ym2610_t *chip, const uint8_t *data, uint32_t size);
void ym2610_set_adpcm_b_rom(ym2610_t *chip, const uint8_t *data, uint32_t size);

/* Timer/IRQ callback */
typedef void (*ym2610_irq_fn)(void *ctx, int irq);
void ym2610_set_irq_callback(ym2610_t *chip, ym2610_irq_fn fn, void *ctx);

#endif /* YM2610_H */

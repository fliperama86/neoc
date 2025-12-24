#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ym2610.h"

/* YM2610 (OPNB) has:
 * - 4 FM channels (2 operators each on channels 1-4, channel 3 can be special)
 * - 3 SSG channels (AY-3-8910 compatible)
 * - 6 ADPCM-A channels (drum samples)
 * - 1 ADPCM-B channel (voice/music samples)
 */

#define FM_CHANNELS     4
#define SSG_CHANNELS    3
#define ADPCM_A_CHANNELS 6

struct ym2610 {
    int clock;
    int sample_rate;

    /* FM registers */
    uint8_t fm_regs[2][256];

    /* SSG registers (AY-3-8910) */
    uint8_t ssg_regs[16];
    uint8_t ssg_addr;

    /* ADPCM-A state */
    struct {
        uint32_t start;
        uint32_t end;
        uint32_t pos;
        uint8_t volume;
        uint8_t playing;
    } adpcm_a[ADPCM_A_CHANNELS];

    /* ADPCM-B state */
    struct {
        uint32_t start;
        uint32_t end;
        uint32_t pos;
        uint32_t delta;
        int16_t signal;
        int16_t step;
        uint8_t playing;
    } adpcm_b;

    /* ADPCM ROMs */
    const uint8_t *adpcm_a_rom;
    uint32_t adpcm_a_size;
    const uint8_t *adpcm_b_rom;
    uint32_t adpcm_b_size;

    /* Timer state */
    uint16_t timer_a;
    uint8_t timer_b;
    uint8_t timer_ctrl;

    /* IRQ callback */
    ym2610_irq_fn irq_callback;
    void *irq_ctx;

    /* Current register address */
    uint8_t addr_a;
    uint8_t addr_b;
};

ym2610_t *ym2610_create(int clock, int sample_rate) {
    ym2610_t *chip = calloc(1, sizeof(ym2610_t));
    if (!chip) return NULL;

    chip->clock = clock;
    chip->sample_rate = sample_rate;

    return chip;
}

void ym2610_destroy(ym2610_t *chip) {
    free(chip);
}

void ym2610_reset(ym2610_t *chip) {
    if (!chip) return;

    memset(chip->fm_regs, 0, sizeof(chip->fm_regs));
    memset(chip->ssg_regs, 0, sizeof(chip->ssg_regs));
    memset(chip->adpcm_a, 0, sizeof(chip->adpcm_a));
    memset(&chip->adpcm_b, 0, sizeof(chip->adpcm_b));

    chip->addr_a = chip->addr_b = 0;
    chip->timer_a = chip->timer_b = 0;
    chip->timer_ctrl = 0;
}

void ym2610_write(ym2610_t *chip, uint8_t addr, uint8_t data) {
    if (!chip) return;

    switch (addr & 3) {
        case 0: /* Address port A */
            chip->addr_a = data;
            break;
        case 1: /* Data port A */
            chip->fm_regs[0][chip->addr_a] = data;
            /* TODO: Process register write */
            break;
        case 2: /* Address port B */
            chip->addr_b = data;
            break;
        case 3: /* Data port B */
            chip->fm_regs[1][chip->addr_b] = data;
            /* TODO: Process register write */
            break;
    }
}

uint8_t ym2610_read(ym2610_t *chip, uint8_t addr) {
    if (!chip) return 0;

    switch (addr & 3) {
        case 0: /* Status 0 */
            return 0; /* TODO: Return timer/busy status */
        case 1: /* SSG data */
            if (chip->ssg_addr < 16) {
                return chip->ssg_regs[chip->ssg_addr];
            }
            return 0;
        case 2: /* Status 1 */
            return 0; /* TODO: Return ADPCM status */
        case 3:
            return 0;
    }

    return 0;
}

void ym2610_update(ym2610_t *chip, int16_t *buffer, int samples) {
    if (!chip || !buffer) return;

    /* TODO: Implement proper FM synthesis and ADPCM decoding */
    /* For now, output silence */
    memset(buffer, 0, samples * 2 * sizeof(int16_t));
}

void ym2610_set_adpcm_a_rom(ym2610_t *chip, const uint8_t *data, uint32_t size) {
    if (!chip) return;
    chip->adpcm_a_rom = data;
    chip->adpcm_a_size = size;
}

void ym2610_set_adpcm_b_rom(ym2610_t *chip, const uint8_t *data, uint32_t size) {
    if (!chip) return;
    chip->adpcm_b_rom = data;
    chip->adpcm_b_size = size;
}

void ym2610_set_irq_callback(ym2610_t *chip, ym2610_irq_fn fn, void *ctx) {
    if (!chip) return;
    chip->irq_callback = fn;
    chip->irq_ctx = ctx;
}

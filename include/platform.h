#ifndef NEOC_PLATFORM_H
#define NEOC_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>
#include "input.h"

/*
 * Platform abstraction layer
 * Provides windowing, input, audio, and timing across all platforms
 */

typedef struct neoc_platform neoc_platform_t;

/* Audio callback type (SDL-style) */
typedef void (*platform_audio_callback_t)(void *userdata, uint8_t *stream, int len);

/* Lifecycle */
neoc_platform_t *platform_create(const char *title, int width, int height);
void platform_destroy(neoc_platform_t *p);

/* Events - returns false when quit requested */
bool platform_poll_events(neoc_platform_t *p, neoc_input_t *input);

/* Video */
void platform_render_frame(neoc_platform_t *p, const uint32_t *pixels, int width, int height);

/* Audio */
void platform_audio_init(neoc_platform_t *p, int sample_rate, platform_audio_callback_t callback, void *userdata);

/* Timing */
void platform_frame_sync(neoc_platform_t *p);
uint64_t platform_get_ticks(void);
uint64_t platform_get_frequency(void);

#endif /* NEOC_PLATFORM_H */

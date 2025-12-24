#ifndef NEOC_H
#define NEOC_H

#include <stdint.h>
#include <stdbool.h>

#include "input.h"

typedef struct neoc neoc_t;

/* Core emulator lifecycle */
neoc_t *neoc_create(void);
void neoc_destroy(neoc_t *emu);
void neoc_reset(neoc_t *emu);

/* ROM loading */
int neoc_load_rom(neoc_t *emu, const char *path);

/* Emulation control */
void neoc_run_frame(neoc_t *emu);
bool neoc_is_running(neoc_t *emu);
void neoc_stop(neoc_t *emu);

/* State management */
int neoc_save_state(neoc_t *emu, const char *path);
int neoc_load_state(neoc_t *emu, const char *path);

/* Access internal components */
neoc_input_t *neoc_get_input(neoc_t *emu);
const uint32_t *neoc_get_framebuffer(neoc_t *emu);

#endif /* NEOC_H */

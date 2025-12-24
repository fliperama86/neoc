#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

/* NeoGeo input system
 * Supports 2 players with joystick + 4 buttons (A/B/C/D)
 * Plus Start and Select (coin)
 */

typedef struct neoc_input neoc_input_t;

/* Button flags */
#define NEOC_BTN_UP     (1 << 0)
#define NEOC_BTN_DOWN   (1 << 1)
#define NEOC_BTN_LEFT   (1 << 2)
#define NEOC_BTN_RIGHT  (1 << 3)
#define NEOC_BTN_A      (1 << 4)
#define NEOC_BTN_B      (1 << 5)
#define NEOC_BTN_C      (1 << 6)
#define NEOC_BTN_D      (1 << 7)
#define NEOC_BTN_START  (1 << 8)
#define NEOC_BTN_SELECT (1 << 9)

neoc_input_t *input_create(void);
void input_destroy(neoc_input_t *input);
void input_reset(neoc_input_t *input);

/* Set button state for a player (0 or 1) */
void input_set_buttons(neoc_input_t *input, int player, uint16_t buttons);

/* Press/release individual buttons */
void input_press(neoc_input_t *input, int player, uint16_t button);
void input_release(neoc_input_t *input, int player, uint16_t button);

/* Read I/O ports (as memory-mapped) */
uint8_t input_read_port(neoc_input_t *input, uint8_t port);

/* System inputs */
void input_insert_coin(neoc_input_t *input, int player);
bool input_get_test_mode(neoc_input_t *input);
void input_set_test_mode(neoc_input_t *input, bool enabled);

#endif /* INPUT_H */

#include <stdlib.h>
#include <string.h>

#include "input.h"

struct neoc_input {
    uint16_t buttons[2];    /* Player 1 and 2 button states */
    uint8_t coins;          /* Coin inputs */
    uint8_t test_mode;      /* Test/service mode */
};

neoc_input_t *input_create(void) {
    neoc_input_t *input = calloc(1, sizeof(neoc_input_t));
    return input;
}

void input_destroy(neoc_input_t *input) {
    free(input);
}

void input_reset(neoc_input_t *input) {
    if (!input) return;

    input->buttons[0] = 0;
    input->buttons[1] = 0;
    input->coins = 0;
    input->test_mode = 0;
}

void input_set_buttons(neoc_input_t *input, int player, uint16_t buttons) {
    if (!input || player < 0 || player > 1) return;
    input->buttons[player] = buttons;
}

void input_press(neoc_input_t *input, int player, uint16_t button) {
    if (!input || player < 0 || player > 1) return;
    input->buttons[player] |= button;
}

void input_release(neoc_input_t *input, int player, uint16_t button) {
    if (!input || player < 0 || player > 1) return;
    input->buttons[player] &= ~button;
}

uint8_t input_read_port(neoc_input_t *input, uint8_t port) {
    if (!input) return 0xFF;

    /* NeoGeo I/O port mapping (accent on active-low logic) */
    switch (port) {
        case 0x00: /* REG_P1CNT - Player 1 controls */
            return ~(input->buttons[0] & 0xFF);

        case 0x01: /* REG_DIPSW - DIP switches */
            return 0xFF;

        case 0x02: /* REG_SYSTYPE - System type */
            return 0x00; /* MVS */

        case 0x04: /* REG_STATUS_A */
            return 0xFF;

        case 0x05: /* REG_P2CNT - Player 2 controls */
            return ~(input->buttons[1] & 0xFF);

        case 0x06: /* REG_STATUS_B */
            return (input->coins << 4) |
                   (input->test_mode ? 0 : 0x80);

        case 0x0D: /* REG_P1CNT_ACTIVE */
            return ~((input->buttons[0] >> 8) & 0xFF);

        case 0x0E: /* REG_P2CNT_ACTIVE */
            return ~((input->buttons[1] >> 8) & 0xFF);
    }

    return 0xFF;
}

void input_insert_coin(neoc_input_t *input, int player) {
    if (!input || player < 0 || player > 1) return;
    input->coins |= (1 << player);
}

bool input_get_test_mode(neoc_input_t *input) {
    return input ? input->test_mode != 0 : false;
}

void input_set_test_mode(neoc_input_t *input, bool enabled) {
    if (input) {
        input->test_mode = enabled ? 1 : 0;
    }
}

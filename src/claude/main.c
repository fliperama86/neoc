#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "neoc.h"
#include "platform.h"
#include "video.h"

#define WINDOW_TITLE "NeoC - NeoGeo Emulator"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom_file>\n", argv[0]);
        fprintf(stderr, "NeoC v0.1.0 - A modern NeoGeo emulator\n");
        return EXIT_FAILURE;
    }

    /* Initialize platform layer */
    neoc_platform_t *platform = platform_create(
        WINDOW_TITLE,
        NEOC_SCREEN_WIDTH,
        NEOC_SCREEN_HEIGHT
    );

    if (!platform) {
        fprintf(stderr, "Failed to initialize platform\n");
        return EXIT_FAILURE;
    }

    /* Create emulator instance */
    neoc_t *emu = neoc_create();
    if (!emu) {
        fprintf(stderr, "Failed to initialize emulator\n");
        platform_destroy(platform);
        return EXIT_FAILURE;
    }

    /* Load ROM */
    if (neoc_load_rom(emu, argv[1]) != 0) {
        fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
        neoc_destroy(emu);
        platform_destroy(platform);
        return EXIT_FAILURE;
    }

    printf("NeoC - NeoGeo Emulator\n");
    printf("Loaded: %s\n", argv[1]);

    /* Main emulation loop */
    while (neoc_is_running(emu)) {
        /* Process input and check for quit */
        if (!platform_poll_events(platform, neoc_get_input(emu))) {
            neoc_stop(emu);
            break;
        }

        /* Run one frame of emulation */
        neoc_run_frame(emu);

        /* Render the frame */
        const u32 *fb = neoc_get_framebuffer(emu);
        if (fb) {
            platform_render_frame(platform, fb, NEOC_SCREEN_WIDTH, NEOC_SCREEN_HEIGHT);
        }

        /* Frame timing */
        platform_frame_sync(platform);
    }

    /* Cleanup */
    neoc_destroy(emu);
    platform_destroy(platform);

    return EXIT_SUCCESS;
}

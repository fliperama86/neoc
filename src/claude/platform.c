#include <stdlib.h>
#include <string.h>

#include "platform.h"

#ifdef NEOC_USE_SDL
#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#ifndef SCALE_FACTOR
#define SCALE_FACTOR 2
#endif

struct neoc_platform {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *framebuffer;
  SDL_AudioDeviceID audio_device;
  bool running;
  uint64_t frame_start;
  double target_frame_time;
};

neoc_platform_t *platform_create(const char *title, int width, int height) {
  neoc_platform_t *p = calloc(1, sizeof(neoc_platform_t));
  if (!p)
    return NULL;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
    free(p);
    return NULL;
  }

  p->window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       width * SCALE_FACTOR, height * SCALE_FACTOR,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (!p->window) {
    SDL_Quit();
    free(p);
    return NULL;
  }

  p->renderer = SDL_CreateRenderer(
      p->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!p->renderer) {
    SDL_DestroyWindow(p->window);
    SDL_Quit();
    free(p);
    return NULL;
  }

  SDL_RenderSetLogicalSize(p->renderer, width, height);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

  p->framebuffer =
      SDL_CreateTexture(p->renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);

  p->running = true;
  p->target_frame_time = 1000.0 / 59.185606; /* NeoGeo refresh rate */

  return p;
}

void platform_destroy(neoc_platform_t *p) {
  if (!p)
    return;

  if (p->audio_device)
    SDL_CloseAudioDevice(p->audio_device);
  if (p->framebuffer)
    SDL_DestroyTexture(p->framebuffer);
  if (p->renderer)
    SDL_DestroyRenderer(p->renderer);
  if (p->window)
    SDL_DestroyWindow(p->window);
  SDL_Quit();
  free(p);
}

bool platform_poll_events(neoc_platform_t *p, neoc_input_t *input) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      p->running = false;
      break;

    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      bool pressed = (event.type == SDL_KEYDOWN);
      uint16_t btn = 0;

      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_UP:
        btn = NEOC_BTN_UP;
        break;
      case SDL_SCANCODE_DOWN:
        btn = NEOC_BTN_DOWN;
        break;
      case SDL_SCANCODE_LEFT:
        btn = NEOC_BTN_LEFT;
        break;
      case SDL_SCANCODE_RIGHT:
        btn = NEOC_BTN_RIGHT;
        break;
      case SDL_SCANCODE_Z:
        btn = NEOC_BTN_A;
        break;
      case SDL_SCANCODE_X:
        btn = NEOC_BTN_B;
        break;
      case SDL_SCANCODE_C:
        btn = NEOC_BTN_C;
        break;
      case SDL_SCANCODE_V:
        btn = NEOC_BTN_D;
        break;
      case SDL_SCANCODE_RETURN:
        btn = NEOC_BTN_START;
        break;
      case SDL_SCANCODE_RSHIFT:
        btn = NEOC_BTN_SELECT;
        break;
      case SDL_SCANCODE_ESCAPE:
        if (pressed)
          p->running = false;
        break;
      default:
        break;
      }

      if (btn && input) {
        if (pressed)
          input_press(input, 0, btn);
        else
          input_release(input, 0, btn);
      }
      break;
    }
    }
  }

  return p->running;
}

void platform_render_frame(neoc_platform_t *p, const uint32_t *pixels,
                           int width, int height) {
  if (!p || !pixels)
    return;

  (void)height;

  SDL_UpdateTexture(p->framebuffer, NULL, pixels,
                    width * (int)sizeof(uint32_t));
  SDL_RenderClear(p->renderer);
  SDL_RenderCopy(p->renderer, p->framebuffer, NULL, NULL);
  SDL_RenderPresent(p->renderer);
}

void platform_audio_init(neoc_platform_t *p, int sample_rate,
                         platform_audio_callback_t callback, void *userdata) {
  if (!p)
    return;

  SDL_AudioSpec want = {.freq = sample_rate,
                        .format = AUDIO_S16SYS,
                        .channels = 2,
                        .samples = 1024,
                        .callback = callback,
                        .userdata = userdata};

  SDL_AudioSpec have;
  p->audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
  if (p->audio_device) {
    SDL_PauseAudioDevice(p->audio_device, 0);
  }
}

void platform_frame_sync(neoc_platform_t *p) {
  if (!p)
    return;
  /* VSync handled by SDL_RENDERER_PRESENTVSYNC */
}

uint64_t platform_get_ticks(void) { return SDL_GetPerformanceCounter(); }

uint64_t platform_get_frequency(void) { return SDL_GetPerformanceFrequency(); }

#else
/* Native platform implementations would go here */
/* For now, provide stubs */

struct neoc_platform {
  bool running;
};

neoc_platform_t *platform_create(const char *title, int width, int height) {
  (void)title;
  (void)width;
  (void)height;
  neoc_platform_t *p = calloc(1, sizeof(neoc_platform_t));
  if (p)
    p->running = true;
  return p;
}

void platform_destroy(neoc_platform_t *p) { free(p); }

bool platform_poll_events(neoc_platform_t *p, neoc_input_t *input) {
  (void)input;
  return p ? p->running : false;
}

void platform_render_frame(neoc_platform_t *p, const uint32_t *pixels,
                           int width, int height) {
  (void)p;
  (void)pixels;
  (void)width;
  (void)height;
}

void platform_audio_init(neoc_platform_t *p, int sample_rate,
                         platform_audio_callback_t callback, void *userdata) {
  (void)p;
  (void)sample_rate;
  (void)callback;
  (void)userdata;
}

void platform_frame_sync(neoc_platform_t *p) { (void)p; }

uint64_t platform_get_ticks(void) { return 0; }

uint64_t platform_get_frequency(void) { return 1; }

#endif /* NEOC_USE_SDL */

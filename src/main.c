#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "neoc.h"

int main() {
  neoc_t *emu = neoc_create();
  neoc_reset(emu);
  printf("Hello, World!\n");
  neoc_destroy(emu);

  return 0;
}

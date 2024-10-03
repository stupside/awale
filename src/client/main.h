#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "mediator.h"

void app(const char *address, const char *name,
         const struct Mediator *mediator);

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage : %s [address] [pseudo]\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct Mediator mediator;

  init_mediator(&mediator);

  // compute_cmd(mediator, const SocketClient *from, const char *cmd)

  app(argv[1], argv[2], &mediator);

  return EXIT_SUCCESS;
}

#pragma endregion
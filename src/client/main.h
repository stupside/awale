#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>

#include "mediator.h"

void app(const char *address, const char *name, const struct Mediator *mediator,
         const struct ClientMediator *clientMediator);

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage : %s [address] [pseudo]\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct Mediator mediator;

  init_mediator(&mediator);

  struct ClientMediator clientMediator;
  init_client_mediator(&clientMediator);

  // compute_cmd(mediator, const SocketClient *from, const char *cmd)

  app(argv[1], argv[2], &mediator, &clientMediator);

  return EXIT_SUCCESS;
}

#endif
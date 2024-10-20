#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>

#include "mediator.h"

#include "handlers/challenge.h"
#include "handlers/chat.h"
#include "handlers/error.h"
#include "handlers/game.h"
#include "handlers/user.h"

void app(const char *address, unsigned int port, const char *name,
         const char *password, const struct ServerMediator *mediator,
         const struct ClientMediator *clientMediator);

int main(int argc, char **argv) {
  if (argc < 5) {
    printf("Usage : %s [address] [port] [pseudo] [password]\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct ServerMediator server_mediator = {
      .handlers = {},
  };

  add_user_cmds(&server_mediator);
  add_chat_cmds(&server_mediator);
  add_game_cmds(&server_mediator);
  add_error_handlers(&server_mediator);
  add_challenge_cmds(&server_mediator);

  struct ClientMediator client_mediator = {
      .handler_c = 0,
  };

  init_client_mediator(&client_mediator);

  const char *address = argv[1];
  unsigned int port = atoi(argv[2]);

  const char *name = argv[3];
  const char *password = argv[4];

  app(address, port, name, password, &server_mediator, &client_mediator);

  return EXIT_SUCCESS;
}

#endif
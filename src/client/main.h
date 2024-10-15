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

void app(const char *address, const char *name, const char *password,
         const struct ServerMediator *mediator,
         const struct ClientMediator *clientMediator);

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage : %s [address] [pseudo] [password]\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct ServerMediator server_mediator = {
      .handlers = {0},
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

  app(argv[1], argv[2], argv[3], &server_mediator, &client_mediator);

  return EXIT_SUCCESS;
}

#endif
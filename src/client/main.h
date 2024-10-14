#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>

#include "lib/cmds/mediator.h"
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

  struct ServerMediator mediator;

  add_user_cmds(&mediator);
  add_chat_cmds(&mediator);
  add_challenge_cmds(&mediator);
  add_game_cmds(&mediator);
  add_error_handlers(&mediator);

  struct ClientMediator clientMediator;
  init_client_mediator(&clientMediator);

  // handle_cmd(mediator, const SocketClient *from, const char *cmd)

  app(argv[1], argv[2], argv[3], &mediator, &clientMediator);

  return EXIT_SUCCESS;
}

#endif
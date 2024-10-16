#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

#include "handlers/challenge.h"
#include "handlers/chat.h"
#include "handlers/game.h"
#include "handlers/user.h"

#include "lib/cmds/mediator.h"

void app(unsigned int port, struct ServerMediator *);

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <port> <database>\n", argv[0]);
    return EXIT_FAILURE;
  }

  unsigned int port = atoi(argv[1]);
  // const char *database = argv[2];

  struct ServerMediator mediator;

  add_user_cmds(&mediator);
  add_chat_cmds(&mediator);
  add_game_cmds(&mediator);
  add_challenge_cmds(&mediator);

  // open_persistor(&mediator.persistor, database);

  // char cmd[2048];
  // unsigned int client_id = 0;

  // while (read_cmd(&mediator.persistor, &client_id, cmd)) {
  //   handle_cmd(&mediator, client_id, cmd, NO_PERSIST);
  // }

  app(port, &mediator);

  // close_persistor(&mediator.persistor);

  return EXIT_SUCCESS;
}
#endif
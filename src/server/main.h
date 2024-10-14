#ifndef MAIN_H
#define MAIN_H

#include "handlers/challenge.h"
#include "handlers/chat.h"
#include "handlers/game.h"
#include "handlers/user.h"

#include "lib/cmds/mediator.h"

void app(struct ServerMediator *);

int main(int argc, char **argv) {

  struct ServerMediator mediator;

  add_user_cmds(&mediator);
  add_chat_cmds(&mediator);
  add_game_cmds(&mediator);
  add_challenge_cmds(&mediator);

  app(&mediator);

  return EXIT_SUCCESS;
}
#endif
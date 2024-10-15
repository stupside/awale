#ifndef GAME_HANDLERS_H
#define GAME_HANDLERS_H

#include <stdio.h>

#include "lib/cmds/mediator.h"
#include "lib/socket/cmd.h"

#include "lib/socket/cmds/game.h"

unsigned int on_game_play(unsigned int client_id, const void *data) {

  const struct GamePlayRes *res = data;

  switch (res->validity) {
  case VALID:
    printf("Your move was valid\n");
    break;
  case INVALID_NOT_PLAYER_ROUND:
    printf("It's not your turn\n");
    break;
  case INVALID_NO_SEEDS_IN_CASE:
    printf("There are no seeds in this case\n");
    break;
  case INVALID_OPONENT_HAS_NO_SEEDS:
    printf("Your opponent has no seeds / Your move takes all of his seeds ! "
           "please play anither move\n");
    break;
  case INVALID:
    printf("Invalid move\n");
    break;
  case INVALID_TARGET_OUT_OF_BOUNDS:
    printf("Invalid target, move out of bonds\n");
    break;
  }

  return 1;
};

unsigned int on_game_leave_event(unsigned int client_id, const void *data) {

  printf("The openent have left the game ! You can no longer play\n");

  return 1;
};

unsigned int on_game_state_event(unsigned int client_id, const void *data) {

  const struct GameStateEvent *event = data;

  printf("Game state: %d\n", event->status);

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      printf("[%2d]", event->grid[i][j]);
    }
    printf("\n");
  }

  return 1;
};

unsigned int on_game_state(unsigned int client_id, const void *data) {

  const struct GameStateRes *res = data;

  printf("Game state: %d\n", res->status);

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      printf("[%2d]", res->grid[i][j]);
    }
    printf("\n");
  }

  return 1;
};

void add_game_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_GAME_PLAY, &on_game_play);
  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);

  register_cmd(mediator, CMD_GAME_LEAVE_EVENT, &on_game_leave_event);
  register_cmd(mediator, CMD_GAME_STATE_EVENT, &on_game_state_event);
}

#endif
#ifndef GAME_HANDLERS_H
#define GAME_HANDLERS_H

#include <stdio.h>

#include "lib/cmds/mediator.h"
#include "lib/socket/cmd.h"

#include "lib/socket/cmds/game.h"
#include "src/client/handlers/user.h"

unsigned int on_game_play(unsigned int client_id, const void *data) {

  const struct GamePlayRes *res = data;
  printf("Move info : ");
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
  printf("\n");

  return 1;
};

unsigned int on_game_leave_event(unsigned int client_id, const void *data) {

  printf("The oponent have left the game ! You can no longer play\n");

  return 1;
};

unsigned int on_game_state_event(unsigned int client_id, const void *data) {

  const struct GameStateEvent *event = data;

  printf("\n\n  Player 0 : score %d\n", event->score[0]);

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    printf("  [%2d] ", event->grid[i][0]);
  }
  printf("\n");

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    printf("-------");
  }
  printf("\n");

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    printf("  [%2d] ", event->grid[i][1]);
  }
  printf("\n  Player 1 : score %d \n\n", event->score[1]);

  if (CLIENT_ID == event->player[event->turn]) {
    printf("It's your turn !\n");
  } else {
    printf("It's your opponent's turn !\n");
  }

  return 1;
};

unsigned int on_game_state(unsigned int client_id, const void *data) {

  const struct GameStateRes *res = data;

  printf("Game state: %d\n", res->status);

  printf("\n\n  Player 0 is %d : score %d\n", res->player[PLAYER1],
         res->score[0]);

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    printf("  [%2d] ", res->grid[i][0]);
  }
  printf("\n");

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    printf("-------");
  }
  printf("\n");

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    printf("  [%2d] ", res->grid[i][1]);
  }
  printf("\n  Player 1 is %d : score %d \n\n", res->player[PLAYER1],
         res->score[1]);
  printf("It's player %d's turn\n", res->turn);

  return 1;
};

unsigned int on_game_observe(unsigned int client_id, const void *data) {

  const struct GameObserveRes *res = data;
  res->observe
      ? printf("You are now observing the game of client %d\n", res->client_id)
      : printf("You are no longer observing the game of client %d\n",
               res->client_id);

  return 1;
};

unsigned int on_game_observe_event(unsigned int client_id, const void *data) {

  const struct GameObserveEvent *event = data;

  event->observe
      ? printf("Client %d is now observing the game\n", event->client_id)
      : printf("Client %d is no longer observing the game\n", event->client_id);

  return 1;
};

void add_game_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_GAME_PLAY, &on_game_play);
  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);
  register_cmd(mediator, CMD_GAME_OBSERVE, &on_game_observe);

  register_cmd(mediator, CMD_GAME_OBSERVE_EVENT, &on_game_observe_event);

  register_cmd(mediator, CMD_GAME_LEAVE_EVENT, &on_game_leave_event);
  register_cmd(mediator, CMD_GAME_STATE_EVENT, &on_game_state_event);
}

#endif
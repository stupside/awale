#include "game.h"

#include "lib/display/colors.h"
#include "lib/socket/cmd.h"
#include "lib/socket/cmds/game.h"

#include "src/client/handlers/user.h"

unsigned int on_game_play(unsigned int client_id, const void *data) {

  const struct GamePlayRes *res = data;
  PRINT_COLOR(COLOR_CYAN, "Move info: ");
  switch (res->validity) {
  case VALID:
    PRINT_COLOR(COLOR_GREEN, "Your move was valid.\n");
    break;
  case INVALID_NOT_PLAYER_ROUND:
    PRINT_COLOR(COLOR_YELLOW, "It's not your turn.\n");
    break;
  case INVALID_NO_SEEDS_IN_CASE:
    PRINT_COLOR(COLOR_RED, "There are no seeds in this case.\n");
    break;
  case INVALID_OPONENT_HAS_NO_SEEDS:
    PRINT_COLOR(COLOR_RED, "Your opponent has no seeds / Your move takes all "
                           "of their seeds! Please play another move.\n");
    break;
  case INVALID:
    PRINT_COLOR(COLOR_RED, "Invalid move.\n");
    break;
  case INVALID_TARGET_OUT_OF_BOUNDS:
    PRINT_COLOR(COLOR_RED, "Invalid target, move out of bounds.\n");
    break;
  }

  return 1;
};

unsigned int on_game_leave(unsigned int client_id, const void *data) {

  PRINT_COLOR(COLOR_RED, "You have left the game\n");

  return 1;
};

unsigned int on_game_leave_event(unsigned int client_id, const void *data) {

  const struct GameLeaveEvent *event = data;

  if (event->observing) {
    PRINT_COLOR(COLOR_RED, "Your opponent has left the game\n");
  } else {
    PRINT_COLOR(COLOR_RED,
                "You are no longer observing the game of client %d\n",
                event->client_id);
  }

  return 1;
};

unsigned int on_game_state_event(unsigned int client_id, const void *data) {

  const struct GameStateEvent *event = data;

  PRINT_COLOR(COLOR_BLUE, "Player 0: score %d\n", event->score[PLAYER1]);

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    PRINT_COLOR(COLOR_GREEN, "  [%2d] ", event->grid[i][0]);
  }

  printf("\n");

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    printf("-------");
  }
  printf("\n");

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    PRINT_COLOR(COLOR_GREEN, "  [%2d] ", event->grid[i][1]);
  }

  printf("\n");

  PRINT_COLOR(COLOR_BLUE, "Player 1: score %d \n\n", event->score[PLAYER2]);

  if (CLIENT_ID == event->player[event->turn]) {
    PRINT_COLOR(COLOR_CYAN, "It's your turn!\n");
  } else {
    PRINT_COLOR(COLOR_CYAN, "It's your opponent's turn!\n");
  }

  return 1;
};

unsigned int on_game_state(unsigned int client_id, const void *data) {
  const struct GameStateRes *res = data;

  PRINT_COLOR(COLOR_CYAN, "Game state: %d\n", res->status);

  PRINT_COLOR(COLOR_BLUE, "Player 0 is %d: score %d\n", res->player[PLAYER1],
              res->score[PLAYER1]);

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    PRINT_COLOR(COLOR_GREEN, "  [%2d] ", res->grid[i][0]);
  }
  printf("\n");

  for (int i = 0; i < GRID_ROWS; i++) {
    printf("-------");
  }

  printf("\n");

  printf("  ");
  for (int i = 0; i < GRID_ROWS; i++) {
    PRINT_COLOR(COLOR_GREEN, "  [%2d] ", res->grid[i][1]);
  }

  printf("\n");

  PRINT_COLOR(COLOR_BLUE, "Player 1 is %d: score %d\n", res->player[PLAYER2],
              res->score[PLAYER2]);

  PRINT_COLOR(COLOR_YELLOW, "It's player %d's turn\n", res->turn);

  return 1;
};

unsigned int on_game_observe(unsigned int client_id, const void *data) {

  const struct GameObserveRes *res = data;

  if (res->observe) {
    PRINT_COLOR(COLOR_YELLOW, "You are now observing the game of client %d\n",
                res->client_id);
  } else {
    PRINT_COLOR(COLOR_YELLOW,
                "You are no longer observing the game of client %d\n",
                res->client_id);
  }

  return 1;
};

unsigned int on_game_observe_event(unsigned int client_id, const void *data) {

  const struct GameObserveEvent *event = data;

  if (event->observe) {
    PRINT_COLOR(COLOR_YELLOW, "Client %d is now observing the game\n",
                event->client_id);
  } else {
    PRINT_COLOR(COLOR_YELLOW, "Client %d is no longer observing the game\n",
                event->client_id);
  }

  return 1;
};

void add_game_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_GAME_PLAY, &on_game_play);
  register_cmd(mediator, CMD_GAME_LEAVE, &on_game_leave);
  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);
  register_cmd(mediator, CMD_GAME_OBSERVE, &on_game_observe);

  register_cmd(mediator, CMD_GAME_OBSERVE_EVENT, &on_game_observe_event);

  register_cmd(mediator, CMD_GAME_LEAVE_EVENT, &on_game_leave_event);
  register_cmd(mediator, CMD_GAME_STATE_EVENT, &on_game_state_event);
}
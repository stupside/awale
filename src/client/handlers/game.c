#include "game.h"

#include "lib/display/colors.h"
#include "lib/socket/cmd.h"
#include "lib/socket/cmds/game.h"

#include "src/client/handlers/user.h"

void print_awale_board(const int grid[GRID_ROWS][GRID_COLS]) {
  printf("  ┌─────────────── Awale Board ───────────────┐\n");
  printf("  │ Player 0: ");
  for (int i = 0; i < 6; i++) {
    printf("%2d ", grid[i][0]);
  }
  printf(" │\n  │ Player 1: ");
  for (int i = 0; i < 6; i++) {
    printf("%2d ", grid[i][1]);
  }
  printf(" │\n  └───────────────────────────────────────────┘\n");
}

unsigned int on_game_play(unsigned int client_id, const void *data) {
  const struct GamePlayRes *res = data;

  PRINT_COLOR(COLOR_CYAN, "🔄 Move Status: ");

  switch (res->validity) {
  case VALID:
    PRINT_COLOR(COLOR_GREEN, "✅ Your move was valid.\n");
    break;
  case INVALID_NOT_PLAYER_ROUND:
    PRINT_COLOR(COLOR_YELLOW, "⚠️ It's not your turn.\n");
    break;
  case INVALID_NO_SEEDS_IN_CASE:
    PRINT_COLOR(COLOR_RED, "❌ This case has no seeds.\n");
    break;
  case INVALID_OPONENT_HAS_NO_SEEDS:
    PRINT_COLOR(COLOR_RED,
                "❌ Your opponent has no seeds, or your move would capture all "
                "their seeds. Please choose another move.\n");
    break;
  case INVALID:
    PRINT_COLOR(COLOR_RED, "❌ Invalid move.\n");
    break;
  case INVALID_TARGET_OUT_OF_BOUNDS:
    PRINT_COLOR(COLOR_RED, "❌ Target is out of bounds.\n");
    break;
  }

  return 1;
}

unsigned int on_game_leave(unsigned int client_id, const void *data) {
  PRINT_COLOR(COLOR_RED, "🚪 You have left the game.\n");
  return 1;
}

unsigned int on_game_leave_event(unsigned int client_id, const void *data) {
  const struct GameLeaveEvent *event = data;

  if (event->observing) {
    PRINT_COLOR(COLOR_RED, "❌ Your opponent has left the game.\n");
  } else {
    PRINT_COLOR(COLOR_RED,
                "🔍 You are no longer observing the game of client %d.\n",
                event->client_id);
  }

  return 1;
}

unsigned int on_game_state_event(unsigned int client_id, const void *data) {
  const struct GameStateEvent *event = data;

  PRINT_COLOR(COLOR_BLUE, "📊 Player 0: Score %d\n", event->score[PLAYER1]);
  print_awale_board(event->grid); // Updated to Awale board representation
  PRINT_COLOR(COLOR_BLUE, "📊 Player 1: Score %d\n\n", event->score[PLAYER2]);

  switch (event->status) {
  case GAME_OVER_PLAYER1_WINS:
    PRINT_COLOR(COLOR_GREEN, "🎉 Player 0 wins!\n");
    break;
  case GAME_OVER_PLAYER2_WINS:
    PRINT_COLOR(COLOR_GREEN, "🎉 Player 1 wins!\n");
    break;
  case GAME_OVER_STALEMATE:
    PRINT_COLOR(COLOR_GREEN, "🎉 It's a draw!\n");
    break;
  case PASS_TURN_NO_SEEDS:
    PRINT_COLOR(COLOR_YELLOW, "🔄 Passing turn, no seeds to play.\n");
    break;
  case GAME_NOT_OVER:
    break;
  }

  if (event->status == GAME_NOT_OVER) {

  } else {
    return 1;
  }

  if (CLIENT_ID == event->player[event->turn]) {
    PRINT_COLOR(COLOR_CYAN, "🕐 It's your turn!\n");
  } else {
    PRINT_COLOR(COLOR_CYAN, "🕐 It's your opponent's turn!\n");
  }

  return 1;
}

unsigned int on_game_state(unsigned int client_id, const void *data) {
  const struct GameStateRes *res = data;

  PRINT_COLOR(COLOR_CYAN, "🎲 Game State: %d\n", res->status);
  PRINT_COLOR(COLOR_BLUE, "Player 0 is %d: Score %d\n", res->player[PLAYER1],
              res->score[PLAYER1]);

  print_awale_board(res->grid); // Updated to Awale board representation

  PRINT_COLOR(COLOR_BLUE, "Player 1 is %d: Score %d\n", res->player[PLAYER2],
              res->score[PLAYER2]);

  switch (res->status) {
  case GAME_OVER_PLAYER1_WINS:
    PRINT_COLOR(COLOR_GREEN, "🎉 Player 0 wins!\n");
    break;
  case GAME_OVER_PLAYER2_WINS:
    PRINT_COLOR(COLOR_GREEN, "🎉 Player 1 wins!\n");
    break;
  case GAME_OVER_STALEMATE:
    PRINT_COLOR(COLOR_GREEN, "🎉 It's a draw!\n");
    break;
  case PASS_TURN_NO_SEEDS:
    PRINT_COLOR(COLOR_YELLOW, "🔄 Passing turn, no seeds to play.\n");
    break;
  case GAME_NOT_OVER:
    break;
  }

  if (res->status == GAME_NOT_OVER) {

  } else {
    return 1;
  }

  PRINT_COLOR(COLOR_YELLOW, "🕐 It's player %d's turn.\n", res->turn);

  return 1;
}

unsigned int on_game_observe(unsigned int client_id, const void *data) {
  const struct GameObserveRes *res = data;

  if (res->observe) {
    PRINT_COLOR(COLOR_YELLOW,
                "👁️ You are now observing the game of client %d.\n",
                res->client_id);
  } else {
    PRINT_COLOR(COLOR_YELLOW,
                "👁️ You stopped observing the game of client %d.\n",
                res->client_id);
  }

  return 1;
}

unsigned int on_game_observe_event(unsigned int client_id, const void *data) {
  const struct GameObserveEvent *event = data;

  if (event->observe) {
    PRINT_COLOR(COLOR_YELLOW, "👁️ Client %d is now observing the game.\n",
                event->client_id);
  } else {
    PRINT_COLOR(COLOR_YELLOW, "👁️ Client %d stopped observing the game.\n",
                event->client_id);
  }

  return 1;
}

void add_game_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_GAME_PLAY, &on_game_play, NO_PERSIST);
  register_cmd(mediator, CMD_GAME_LEAVE, &on_game_leave, NO_PERSIST);
  register_cmd(mediator, CMD_GAME_STATE, &on_game_state, NO_PERSIST);
  register_cmd(mediator, CMD_GAME_OBSERVE, &on_game_observe, NO_PERSIST);

  register_cmd(mediator, CMD_GAME_OBSERVE_EVENT, &on_game_observe_event,
               NO_PERSIST);
  register_cmd(mediator, CMD_GAME_LEAVE_EVENT, &on_game_leave_event,
               NO_PERSIST);
  register_cmd(mediator, CMD_GAME_STATE_EVENT, &on_game_state_event,
               NO_PERSIST);
}

#ifndef GAME_HANDLERS_H
#define GAME_HANDLERS_H

#include "lib/cmds/mediator.h"

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/game.h"

unsigned int on_game_state(unsigned int client_id, const void *data) {

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  const struct Lobby *lobby = find_running_lobby(awale_server(), client);

  if (!lobby) {
    return 0;
  }

  struct GameStateRes res = {
      .status = status(&lobby->awale),
  };

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      res.grid[i][j] = lobby->awale.grid[i][j];
    }
  }

  send_cmd_to(client->socket, CMD_GAME_STATE, &res,
              sizeof(struct GameStateRes));

  return 1;
};

unsigned int on_game_play(unsigned int client_id, const void *data) {

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  const struct GamePlayReq *req = data;

  struct Lobby *lobby = find_running_lobby(awale_server(), client);

  if (!lobby) {
    return 0;
  }

  enum CoupValidity ok = play(&lobby->awale, client_id, req->input);

  if (ok != VALID) {

    const struct GamePlayRes event = {
        .validity = ok}; // Do no send two messages successively

    send_cmd_to(client->socket, CMD_GAME_PLAY, &event,
                sizeof(struct GamePlayRes));
    return 0;
  }

  struct GameStateEvent res = {
      .status = status(&lobby->awale),
  };

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      res.grid[i][j] = lobby->awale.grid[i][j];
    }
  }

  send_cmd_to(lobby->client[PLAYER1]->socket, CMD_GAME_STATE_EVENT, &res,
              sizeof(struct GameStateEvent));

  send_cmd_to(lobby->client[PLAYER2]->socket, CMD_GAME_STATE_EVENT, &res,
              sizeof(struct GameStateEvent));

  return 1;
};

void add_game_cmds(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_GAME_PLAY, &on_game_play);
  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);
}

#endif
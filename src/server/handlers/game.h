#ifndef GAME_HANDLERS_H
#define GAME_HANDLERS_H

#include "lib/cmds/mediator.h"

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/error.h"
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

  return awale_play(awale_server(), client, req->input);
};

unsigned int on_game_observe(unsigned int client_id, const void *data) {

  const SocketClient *observer =
      find_client_by_id(&awale_server()->pool, client_id);

  const struct GameObserveReq *req = data;

  const SocketClient *observed =
      find_client_by_id(&awale_server()->pool, req->client_id);

  struct Lobby *lobby = find_running_lobby(awale_server(), observed);

  if (!lobby) {
    return 0;
  }

  unsigned int ok = observe_lobby(lobby, observer, req->observe);

  if (!ok) {
    struct ErrorEvent event = {
        .message = "An error occured while trying to observe the game",
    };

    send_cmd_to(observer->socket, CMD_ERROR_EVENT, &event,
                sizeof(struct ErrorEvent));

    return 0;
  }

  const struct GameObserveRes res = {
      .observe = req->observe,
      .client_id = req->client_id,
  };

  send_cmd_to(observer->socket, CMD_GAME_OBSERVE, &res,
              sizeof(struct GameObserveRes));

  const struct GameObserveEvent event = {
      .client_id = client_id,
      .observe = req->observe,
  };

  send_cmd_to(lobby->client[PLAYER1]->socket, CMD_GAME_OBSERVE_EVENT, &event,
              sizeof(struct GameObserveEvent));

  send_cmd_to(lobby->client[PLAYER2]->socket, CMD_GAME_OBSERVE_EVENT, &event,
              sizeof(struct GameObserveEvent));

  return 1;
};

void add_game_cmds(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_GAME_PLAY, &on_game_play);
  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);
  register_cmd(mediator, CMD_GAME_OBSERVE, &on_game_observe);
}

#endif
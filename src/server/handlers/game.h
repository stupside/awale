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
    send_error_to_client(client, ERROR_GET_GAME_NOT_RUNNING);
    return 0;
  }

  struct GameStateRes res = {
      .status = status(&lobby->awale),
      .turn = lobby->awale.current,
      .score = {lobby->awale.score[PLAYER1], lobby->awale.score[PLAYER2]},
      .player = {lobby->client[PLAYER1]->id, lobby->client[PLAYER2]->id},
  };

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      res.grid[i][j] = lobby->awale.grid[i][j];
    }
  }

  send_cmd_to_client(client, CMD_GAME_STATE, &res, sizeof(struct GameStateRes));

  return 1;
};

unsigned int on_game_play(unsigned int client_id, const void *data) {

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  const struct GamePlayReq *req = data;

  unsigned int ok = awale_play(awale_server(), client, req->input);

  if (!ok) {
    send_error_to_client(client, ERROR_FAILED_TO_PLAY);
    return 0;
  }

  return 1;
};

unsigned int on_game_observe(unsigned int client_id, const void *data) {

  const SocketClient *observer =
      find_client_by_id(&awale_server()->pool, client_id);

  const struct GameObserveReq *req = data;

  const SocketClient *observed =
      find_client_by_id(&awale_server()->pool, req->client_id);

  struct Lobby *lobby = find_running_lobby(awale_server(), observed);

  if (!lobby) {
    send_error_to_client(observer, ERROR_OBSRRVE_GAME_NOT_RUNNING);
    return 0;
  }

  unsigned int ok = observe_lobby(lobby, observer, req->observe);

  if (ok) {
    return 1;
  }

  send_error_to_client(observer, ERROR_OBSERVE_GAME_ERROR);

  return 0;
};

unsigned int on_game_leave(unsigned int client_id, const void *data) {

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  struct Lobby *lobby = find_running_lobby(awale_server(), client);

  if (!lobby) {

    send_error_to_client(client, ERROR_LEAVE_GAME_NOT_RUNNING);

    return 0;
  }

  end_lobby(lobby);

  const struct GameLeaveRes res = {};

  const enum PlayerID player_id =
      lobby->client[PLAYER1]->id == client_id ? PLAYER1 : PLAYER2;

  send_cmd_to_client(lobby->client[player_id], CMD_GAME_LEAVE, &res,
                     sizeof(struct GameLeaveRes));

  struct GameLeaveEvent event = {
      .observing = 0,
      .client_id = client_id,
  };

  const enum PlayerID other_player_id =
      player_id == PLAYER1 ? PLAYER2 : PLAYER1;

  send_cmd_to_client(lobby->client[other_player_id], CMD_GAME_LEAVE_EVENT,
                     &event, sizeof(struct GameLeaveEvent));

  event.observing = 1;

  for (unsigned int idx = 0; idx < MAX_CLIENTS; idx++) {
    const struct SocketClient *observator = lobby->observators[idx];

    if (observator) {
      send_cmd_to_client(observator, CMD_GAME_LEAVE_EVENT, &event,
                         sizeof(struct GameLeaveEvent));
    }
  }

  return 1;
};

void add_game_cmds(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_GAME_PLAY, &on_game_play, PERSIST);
  register_cmd(mediator, CMD_GAME_LEAVE, &on_game_leave, PERSIST);
  register_cmd(mediator, CMD_GAME_STATE, &on_game_state, NO_PERSIST);
  register_cmd(mediator, CMD_GAME_OBSERVE, &on_game_observe, PERSIST);
}

#endif
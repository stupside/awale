#include "server.h"

#include "lib/awale/awale.h"
#include "lib/socket/pool.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmd.h"
#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/game.h"

#include <stdlib.h>

struct Server *awale_server() {

  static struct Server server = {.pool = (SocketPool){
                                     .count = 0,
                                 }};

  return &server;
}

struct Lobby *find_lobby(struct Server *server,
                         const struct SocketClient *client) {

  for (int i = 0; i < MAX_LOBBIES; i++) {
    struct Lobby *lobby = &server->lobbies[i];

    if (lobby->client[PLAYER1]->id == client->id ||
        lobby->client[PLAYER2]->id == client->id) {

      if (lobby->state != LOBBY_STATE_FINISHED) {
        return lobby;
      }

      return NULL;
    }
  }

  return NULL;
}

int challenge(struct Server *server, SocketClient *p1, SocketClient *p2) {

  static unsigned int lobbies = 0;

  if (lobbies == MAX_LOBBIES) {
    return 0;
  }

  if (find_lobby(server, p1) == NULL && find_lobby(server, p2) == NULL) {
    // Both clients are not in a game
  } else {
    return 0;
  }

  server->lobbies[lobbies++] = new_lobby(p1, p2);

  const struct ChallengeEvent event = {
      .client_id = p1->id,
  };

  const char *cmd =
      inline_cmd(CMD_CHALLENGE, &event, sizeof(struct ChallengeEvent));

  write_to_socket(p2->sock, cmd);

  free(&cmd);

  return 1;
}

int awale_play(struct Server *server, const SocketClient *client, int target) {

  struct Lobby *lobby = find_lobby(server, client);

  if (lobby == NULL) {
    return 0;
  }

  if (lobby->state != LOBBY_STATE_PLAYING) {
    return 0;
  }

  const enum PlayerID player =
      lobby->client[PLAYER1]->id == client->id ? PLAYER1 : PLAYER2;

  const enum CoupValidity validity = play(&lobby->awale, player, target);

  struct GamePlayRes res = {
      .validity = validity,
  };

  char *cmd = inline_cmd(CMD_GAME_PLAY, &res, sizeof(struct GamePlayRes));

  write_to_socket(client->sock, cmd);

  free(&cmd);

  if (validity == VALID) {

    struct GameStateEvent event = {
        .status = status(&lobby->awale),
    };

    for (int i = 0; i < GRID_ROWS; i++) {
      for (int j = 0; j < GRID_COLS; j++) {
        event.grid[i][j] = lobby->awale.grid[i][j];
      }
    }

    char *cmd =
        inline_cmd(CMD_GAME_STATE, &event, sizeof(struct GameStateEvent));

    write_to_socket(lobby->client[PLAYER1]->sock, cmd);
    write_to_socket(lobby->client[PLAYER2]->sock, cmd);

    free(&cmd);
  }

  return 0;
}

int handle_challenge(struct Server *server, const SocketClient *client,
                     int accept) {

  struct Lobby *lobby = find_lobby(server, client);

  if (lobby == NULL) {
    return 0;
  }

  if (lobby->state != LOBBY_STATE_WAITING) {
    return 0;
  }

  if (lobby->client[PLAYER2]->id == client->id) {

    if (!accept) {
      end_lobby(lobby);
    } else {
      start_lobby(lobby);
    }

    struct ChallengeHandleEvent event = {
        .accept = accept,
    };

    char *cmd = inline_cmd(CMD_CHALLENGE_HANDLE, &event,
                           sizeof(struct ChallengeHandleEvent));

    write_to_socket(lobby->client[PLAYER1]->sock, cmd);

    free(&cmd);
  }

  return 0;
};
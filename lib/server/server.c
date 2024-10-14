#include "server.h"

#include "lib/awale/awale.h"
#include "lib/socket/pool.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmd.h"
#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/game.h"

#include <stdio.h>
#include <stdlib.h>

struct Server *awale_server() {

  static struct Server server;

  return &server;
}

void init_awale_server(struct Server *server) {

  server->lobbies_c = 0;
  server->lobbies = malloc(MAX_LOBBIES * sizeof(struct Lobby));
}

unsigned int is_lobby_player(const struct Lobby *lobby, const SocketClient *p) {

  return lobby->client[PLAYER1]->id == p->id ||
         lobby->client[PLAYER2]->id == p->id;
}

struct Lobby *find_waiting_lobby(struct Server *server,
                                 const SocketClient *challenger,
                                 const SocketClient *challenged) {

  for (unsigned int i = 0; i < server->lobbies_c; i++) {

    struct Lobby *lobby = &server->lobbies[i];

    if (!lobby) {
      continue;
    }

    if (lobby->state == LOBBY_STATE_WAITING) {

      if (lobby->client[PLAYER1]->id == challenger->id &&
          lobby->client[PLAYER2]->id == challenged->id) {
        return lobby;
      }

      if (lobby->client[PLAYER1]->id == challenged->id &&
          lobby->client[PLAYER2]->id == challenger->id) {
        return lobby;
      }
    }
  }

  return NULL;
}

struct Lobby *find_running_lobby(struct Server *server,
                                 const SocketClient *client) {

  for (unsigned int i = 0; i < server->lobbies_c; i++) {

    struct Lobby *lobby = &server->lobbies[i];

    if (!lobby) {
      continue;
    }

    if (lobby->state == LOBBY_STATE_RUNNING) {

      if (is_lobby_player(lobby, client)) {
        return lobby;
      }
    }
  }

  return NULL;
}

int challenge(struct Server *server, SocketClient *challenger,
              SocketClient *challenged) {

  if (challenger->id == challenged->id) {
    return 0;
  }

  if (find_running_lobby(server, challenger) ||
      find_running_lobby(server, challenged)) {
    return 0;
  }

  const struct Lobby *waiting_lobby =
      find_waiting_lobby(server, challenger, challenged);

  if (waiting_lobby) {
    return 0;
  }

  if (server->lobbies_c == MAX_LOBBIES) {
    return 0;
  }

  server->lobbies[server->lobbies_c++] = new_lobby(challenger, challenged);

  const struct ChallengeEvent event = {
      .client_id = challenger->id,
  };

  send_cmd_to(challenged->socket, CMD_CHALLENGE_EVENT, &event,
              sizeof(struct ChallengeEvent));

  return 1;
}

int awale_play(struct Server *server, const SocketClient *client, int target) {

  struct Lobby *lobby = find_running_lobby(server, client);

  if (!lobby) {
    return 0;
  }

  const enum PlayerID player =
      lobby->client[PLAYER1]->id == client->id ? PLAYER1 : PLAYER2;

  const enum CoupValidity validity = play(&lobby->awale, player, target);

  struct GamePlayRes res = {
      .validity = validity,
  };

  send_cmd_to(client->socket, CMD_GAME_PLAY, &res, sizeof(struct GamePlayRes));

  if (validity == VALID) {

    struct GameStateEvent event = {
        .status = status(&lobby->awale),
    };

    for (int i = 0; i < GRID_ROWS; i++) {
      for (int j = 0; j < GRID_COLS; j++) {
        event.grid[i][j] = lobby->awale.grid[i][j];
      }
    }

    send_cmd_to(lobby->client[PLAYER1]->socket, CMD_GAME_STATE_EVENT, &event,
                sizeof(struct GameStateEvent));

    send_cmd_to(lobby->client[PLAYER2]->socket, CMD_GAME_STATE_EVENT, &event,
                sizeof(struct GameStateEvent));

    for (int i = 0; i < MAX_CLIENTS; i++) {

      const struct SocketClient *observator = lobby->observators[i];

      if (observator) {
        send_cmd_to(observator->socket, CMD_GAME_STATE, &event,
                    sizeof(struct GameStateEvent));
      }
    }

    return 1;
  }

  return 0;
}

int handle_challenge(struct Server *server, const SocketClient *challenger,
                     const SocketClient *challenged, int accept) {

  struct Lobby *lobby = find_waiting_lobby(server, challenger, challenged);

  if (lobby == NULL) {
    return 0;
  }

  if (lobby->client[PLAYER2]->id == challenged->id) {

    if (!accept) {
      end_lobby(lobby);
    } else {
      start_lobby(lobby);
    }

    struct ChallengeHandleEvent event = {
        .accept = accept,
    };

    send_cmd_to(lobby->client[PLAYER1]->socket, CMD_CHALLENGE_HANDLE_EVENT,
                &event, sizeof(struct ChallengeHandleEvent));
  }

  return 0;
};
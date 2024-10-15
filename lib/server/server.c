#include "server.h"

#include "lib/awale/awale.h"
#include "lib/socket/pool.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmd.h"
#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  const struct ChallengeRes res = {};

  send_cmd_to_client(challenger, CMD_CHALLENGE, &res,
                     sizeof(struct ChallengeRes));

  const struct ChallengeEvent event = {
      .client_id = challenger->id,
  };

  send_cmd_to_client(challenged, CMD_CHALLENGE_EVENT, &event,
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

  if (status(&lobby->awale) != GAME_NOT_OVER) {
    lobby->state = LOBBY_STATE_FINISHED;
  }

  struct GamePlayRes res = {
      .validity = validity,
  };

  send_cmd_to_client(client, CMD_GAME_PLAY, &res, sizeof(struct GamePlayRes));

  struct GameStateEvent event = {
      .observing = 0,
      .score =
          {
              lobby->awale.score[PLAYER1],
              lobby->awale.score[PLAYER2],
          },
      .player =
          {
              lobby->client[PLAYER1]->id,
              lobby->client[PLAYER2]->id,
          },
      .status = status(&lobby->awale),
      .turn = lobby->awale.current,
  };

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      event.grid[i][j] = lobby->awale.grid[i][j];
    }
  }

  for (int i = 0; i < PLAYER_COUNT; i++) {
    const struct SocketClient *player = lobby->client[i];

    struct UserRes user = {
        .client_id = player->id,
    };

    strncpy(user.name, player->name, USER_NAME_LEN);

    event.users[i] = user;
  }

  const enum PlayerID other_player_id =
      lobby->client[PLAYER1]->id == client->id ? PLAYER2 : PLAYER1;

  send_cmd_to_client(lobby->client[other_player_id], CMD_GAME_STATE_EVENT,
                     &event, sizeof(struct GameStateEvent));

  event.observing = 1;

  for (unsigned int i = 0; i < MAX_CLIENTS; i++) {

    const struct SocketClient *observator = lobby->observators[i];

    if (observator) {

      if (observator->online) {
        send_cmd_to_client(observator, CMD_GAME_STATE_EVENT, &event,
                           sizeof(struct GameStateEvent));
      }
    }
  }

  return validity == VALID ? 1 : 0;
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

    struct ChallengeHandleRes res = {};

    send_cmd_to_client(lobby->client[PLAYER2], CMD_CHALLENGE, &res,
                       sizeof(struct ChallengeHandleRes));

    struct ChallengeHandleEvent event = {
        .accept = accept,
    };

    send_cmd_to_client(lobby->client[PLAYER1], CMD_CHALLENGE_HANDLE_EVENT,
                       &event, sizeof(struct ChallengeHandleEvent));

    return 1;
  }

  return 0;
};
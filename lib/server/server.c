#include "server.h"

#include "lib/awale/awale.h"

struct Server *awale_server() {

  static struct Server server = {.pool = (SocketPool){
                                     .count = 0,
                                 }};

  return &server;
}

struct Lobby *find_lobby(struct Server *server, const SocketClient *client) {
  for (int i = 0; i < MAX_LOBBIES; i++) {
    struct Lobby *game = &server->lobbies[i];

    if (game->players[PLAYER1].client == client ||
        game->players[PLAYER2].client == client) {
      return game;
    }
  }

  return NULL;
}

int challenge(struct Server *server, const SocketClient *c1,
              const SocketClient *c2) {

  static unsigned int lobbies = 0;

  if (lobbies == MAX_LOBBIES) {
    return 0;
  }

  if (find_lobby(server, c1) == NULL && find_lobby(server, c2) == NULL) {
    // Both clients are not in a game
  } else {
    return 0;
  }

  server->lobbies[lobbies++] = (struct Lobby){
      .state = LOBBY_STATE_WAITING,
      .players =
          {
              [PLAYER1] = {.client = c1},
              [PLAYER2] = {.client = c2},
          },
  };

  return 1;
}

int handle_challenge(struct Server *server, const SocketClient *client,
                     int accept) {

  struct Lobby *lobby = find_lobby(server, client);

  if (lobby == NULL) {
    return 0;
  }

  if (lobby->players[PLAYER2].client == client) {
    if (accept) {
      lobby->state = LOBBY_STATE_PLAYING;
    } else {
      lobby->state = LOBBY_STATE_FINISHED;
    }

    return 1;
  }

  return 0;
}
#include "server.h"

#include "lib/awale/awale.h"
#include "lib/socket/pool.h"

#include "lib/socket/cmd.h"
#include "lib/socket/cmds/challenge.h"
#include "lib/socket/socket.h"

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

int challenge(struct Server *server, SocketClient *c1, SocketClient *c2) {

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
      .client =
          {
              [PLAYER1] = c1,
              [PLAYER2] = c2,
          },
  };

  {
    const struct ChallengeEvent event = {
        .client_id = c1->id,
    };

    const char *cmd =
        inline_cmd(CMD_CHALLENGE, &event, sizeof(struct ChallengeEvent));

    write_to_socket(c2->sock, cmd);

    free(&cmd);
  }

  return 1;
}

int handle_challenge(struct Server *server, const SocketClient *client,
                     int accept) {

  struct Lobby *lobby = find_lobby(server, client);

  if (lobby == NULL) {
    return 0;
  }

  if (lobby->client[PLAYER2]->id == client->id) {
    if (accept) {
      lobby->state = LOBBY_STATE_PLAYING;
    } else {
      lobby->state = LOBBY_STATE_FINISHED;
    }

    return 1;
  }

  return 0;
}
#include "lobby.h"

#include <stdlib.h>

#include "lib/socket/cmd.h"
#include "lib/socket/cmds/challenge.h"
#include "lib/socket/socket.h"

struct Lobby new_lobby(struct SocketClient *p1, struct SocketClient *p2) {

  return (struct Lobby){
      .client = {p1, p2},
      .awale = new_awale(),
      .state = LOBBY_STATE_WAITING,
  };
}

int start_lobby(struct Lobby *lobby) {

  if (lobby->state == LOBBY_STATE_WAITING) {
    reset(&lobby->awale);
  } else {
    return 0;
  }

  lobby->state = LOBBY_STATE_RUNNING;

  return 1;
}

int observe_lobby(struct Lobby *lobby, struct SocketClient *client,
                  unsigned int observe) {

  static unsigned int id;

  for (int i = 0; i < MAX_CLIENTS; i++) {
    const struct SocketClient *observator = lobby->observators[i];

    if (observe) {
      if (observator) {
        if (observator->id == client->id) {
          return 0; // Already observing
        }
      } else {
        lobby->observators[id++] = client;
        return 1;
      }
    } else {
      if (observator) {
        if (observator->id == client->id) {
          lobby->observators[i] = NULL;
          return 1;
        }
      }
    }
  }

  return 0;
}

void end_lobby(struct Lobby *lobby) { lobby->state = LOBBY_STATE_FINISHED; }

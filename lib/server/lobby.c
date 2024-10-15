#include "lobby.h"
#include "server.h"

#include <stdlib.h>

struct Lobby new_lobby(struct SocketClient *challenger,
                       struct SocketClient *challenged) {

  return (struct Lobby){
      .awale = new_awale(),
      .state = LOBBY_STATE_WAITING,
      .client = {challenger, challenged},
      .observators = {NULL},
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

int observe_lobby(struct Lobby *lobby, const struct SocketClient *client,
                  unsigned int observe) {

  if (lobby->state != LOBBY_STATE_RUNNING) {
    return 0;
  }

  unsigned int null_idx = 0;

  for (unsigned int idx = 0; idx < MAX_LOBBIES; idx++) {

    const struct SocketClient *observator = lobby->observators[idx];

    if (observator) {
      if (observator->id == client->id) {
        if (observe) {
          return 0;
        } else {
          lobby->observators[idx] = NULL;
        }
        return 1;
      }
    } else {
      null_idx = idx;
    }
  }

  lobby->observators[null_idx] = client;

  return 0;
}

void end_lobby(struct Lobby *lobby) { lobby->state = LOBBY_STATE_FINISHED; }

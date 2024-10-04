#include "lobby.h"

#include <stdlib.h>

#include "lib/socket/socket.h"

struct Lobby *new_lobby(struct SocketClient *p1, struct SocketClient *p2) {

  struct Lobby *lobby = malloc(sizeof(struct Lobby));

  lobby->client[PLAYER1] = p1;
  lobby->client[PLAYER2] = p2;

  lobby->awale = new_awale();
  lobby->state = LOBBY_STATE_WAITING;

  return lobby;
}

int start_lobby(struct Lobby *lobby) {

  if (lobby->state == LOBBY_STATE_WAITING) {
    reset(&lobby->awale);
  } else {
    return 0;
  }

  lobby->state = LOBBY_STATE_PLAYING;

  return 1;
}

int play_lobby(struct Lobby *lobby, const struct SocketClient *client,
               int target) {

  if (lobby->state == LOBBY_STATE_PLAYING) {

    const enum PlayerID player =
        lobby->client[PLAYER1]->id == client->id ? PLAYER1 : PLAYER2;

    const enum CoupValidity validity = play(&lobby->awale, player, target);

    if (validity == VALID) {

      if (status(&lobby->awale) != GAME_NOT_OVER) {
        lobby->state = LOBBY_STATE_FINISHED;
      }

      return 1;
    }
  }

  return 0;
}

void end_lobby(struct Lobby *lobby) { free(lobby); }

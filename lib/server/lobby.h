#ifndef LOBBY_H
#define LOBBY_H

#include "lib/awale/awale.h"

#define PLAYER_COUNT 2

enum LobbyState {
  LOBBY_STATE_WAITING,
  LOBBY_STATE_PLAYING,
  LOBBY_STATE_FINISHED,
};

struct Lobby {
  enum LobbyState state;

  struct Awale awale;

  struct SocketClient *client[PLAYER_COUNT];
};

struct Lobby *new_lobby(struct SocketClient *p1, struct SocketClient *p2);

int start_lobby(struct Lobby *lobby);

int play_lobby(struct Lobby *lobby, const struct SocketClient *client,
               int target);

void end_lobby(struct Lobby *lobby);

#endif
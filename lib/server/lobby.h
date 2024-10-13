#ifndef LOBBY_H
#define LOBBY_H

#include "lib/awale/awale.h"

#include "lib/socket/pool.h"

#define PLAYER_COUNT 2

enum LobbyState {
  LOBBY_STATE_WAITING,
  LOBBY_STATE_RUNNING,
  LOBBY_STATE_FINISHED,
};

struct Lobby {

  unsigned int id;

  struct Awale awale;

  enum LobbyState state;

  struct SocketClient *client[PLAYER_COUNT];
  struct SocketClient *observators[MAX_CLIENTS];
};

struct Lobby new_lobby(struct SocketClient *challenger,
                       struct SocketClient *challenged);

int start_lobby(struct Lobby *lobby);

int observe_lobby(struct Lobby *lobby, struct SocketClient *client,
                  unsigned int observe);

void end_lobby(struct Lobby *lobby);

#endif
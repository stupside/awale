#pragma once

#include "player.h"

#define PLAYER_COUNT 2

enum LobbyState {
  LOBBY_STATE_WAITING,
  LOBBY_STATE_PLAYING,
  LOBBY_STATE_FINISHED,
} GameState;

struct Lobby {
  enum LobbyState state;

  struct Player players[PLAYER_COUNT];
} Lobby;

#pragma endregion
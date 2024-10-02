#pragma once

#include "player.h"

#define PLAYER_COUNT 2

enum LobbyState {
  WAITING,
  PLAYING,
  FINISHED,
} GameState;

struct Lobby {
  enum LobbyState state;

  struct Player players[PLAYER_COUNT];
} Lobby;

#pragma endregion
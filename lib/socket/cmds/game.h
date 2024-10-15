#ifndef GAME_H
#define GAME_H

#include "lib/awale/awale.h"
#include "lib/socket/cmds/user.h"

struct GamePlayReq {
  unsigned int input;
};
struct GamePlayRes {
  enum CoupValidity validity;
};

struct GameLeaveReq {};
struct GameLeaveEvent {
  ClientId client_id;
};

struct GameListReq {};

struct GameStateReq {};
struct GameStateRes {
  enum GameStatus status;
  int grid[GRID_ROWS][GRID_COLS];
};
struct GameStateEvent {
  unsigned int observed;
  struct UserRes users[2];
  enum GameStatus status;
  int grid[GRID_ROWS][GRID_COLS];
};

struct GameObserveReq {
  ClientId client_id;
  unsigned int observe;
};

struct GameObserveRes {
  ClientId client_id;
  unsigned int observe;
};

struct GameObserveEvent {
  ClientId client_id;
  unsigned int observe;
};

#endif
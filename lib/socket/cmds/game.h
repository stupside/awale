#ifndef GAME_H
#define GAME_H

#include "lib/awale/awale.h"

struct GamePlayReq {
  int input;
};
struct GamePlayRes {
  enum CoupValidity validity;
};

struct GameLeaveReq {};
// struct GameLeaveRes {};

struct GameListReq {};
// struct GameListRes {};

struct GameStateReq {};
struct GameStateRes {
  enum GameStatus status;
  int grid[GRID_ROWS][GRID_COLS];
};
struct GameStateEvent {
  enum GameStatus status;
  int grid[GRID_ROWS][GRID_COLS];
};

#endif
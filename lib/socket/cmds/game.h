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

struct GameListReq {};

struct GameStateReq {};
struct GameStateRes {
  enum GameStatus status;
  int grid[GRID_ROWS][GRID_COLS];
};
struct GameStateEvent { // sent to players when a game state changes
  enum GameStatus status;
  int grid[GRID_ROWS][GRID_COLS];
  struct UserRes users[2];
  unsigned int observed;
};

struct GameObserveReq { // sent to server when a player wants to observe a game
  unsigned int observe;
  ClientId client_id;
};

struct GameObserveRes {
  unsigned int observe;
  ClientId client_id;
};

struct GameObserveEvent { // sent to players when a new observer joins
  ClientId client_id;
  unsigned int observe;
};

#endif
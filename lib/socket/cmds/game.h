#ifndef GAME_H

struct GamePlayReq {
  int input;
};
// struct GamePlayRes {};

struct GameLeaveReq {};
// struct GameLeaveRes {};

struct GameListReq {};
// struct GameListRes {};

struct GameGridReq {};
struct GameGridRes {
  int grid[6][2];
};

#endif
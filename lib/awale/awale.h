#ifndef AWALE_H
#define AWALE_H

#define PLAYER_COUNT 2

#define GRID_ROWS 6
#define GRID_COLS 2

#define MAX_SCORE 25

// La définition de ce cette version du jeu Awale est basée sur les règles
// expliquées sur le site suivante: http://archi02.free.fr/_pages/awele.html
// Dans cette version les règles principales sont les suivantes:

enum CoupValidity {
  VALID,
  INVALID,
  INVALID_NO_SEEDS_IN_CASE,
  INVALID_OPONENT_HAS_NO_SEEDS,
  INVALID_NOT_PLAYER_ROUND,
  INVALID_TARGET_OUT_OF_BOUNDS,
};

enum GameStatus {
  PASS_TURN_NO_SEEDS,

  GAME_OVER_STALEMATE,

  GAME_OVER_PLAYER1_WINS, // +25 graines
  GAME_OVER_PLAYER2_WINS,

  GAME_NOT_OVER,
};

enum PlayerID { PLAYER1, PLAYER2 };

struct Awale {

  enum PlayerID current;

  int score[PLAYER_COUNT];

  int grid[GRID_ROWS][GRID_COLS];
};

struct Awale new_awale();

void reset(struct Awale *awale);

enum CoupValidity play(struct Awale *awale, enum PlayerID player, int target);

enum GameStatus status(const struct Awale *awale);
int winner(const struct Awale *awale, enum PlayerID *player);

enum PlayerID next_player(const struct Awale *awale);

#endif
#include "awale.h"
#include <stdlib.h>
#include <time.h>

struct Awale new_awale() { return (struct Awale){.grid = {}, .score = {0}}; }

void reset(struct Awale *awale) {

  awale->score[PLAYER1] = 0;
  awale->score[PLAYER2] = 0;

  unsigned int seed = time(NULL);

  srand(seed);

  awale->current = rand() % PLAYER_COUNT;

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      awale->grid[i][j] = 4;
    }
  }
}

int player_has_seeds(enum PlayerID player,
                     const int grid[GRID_ROWS][GRID_COLS]) {

  for (int i = 0; i < GRID_ROWS; i++) {
    if (grid[i][player] > 0) {
      return 1;
    }
  }

  return 0;
}

void copy_grid(const struct Awale *awale, int copy[GRID_ROWS][GRID_COLS]) {
  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      copy[i][j] = awale->grid[i][j];
    }
  }
}

void sow_seeds(enum PlayerID player, int grid[GRID_ROWS][GRID_COLS],
               int target) {

  int seeds = grid[target][player];

  enum PlayerID adversaire = (player + 1) % 2;

  grid[target][player] = 0;

  int begin = target;
  int current = begin;

  int line = player;

  while (seeds > 0) {

    if (line == 1) {

      current++;

      if (current == GRID_ROWS) {
        line = 0;
        current = 5;
      }

      // on vérifie si on ne tombe sur la case de départ pour semer une graine
      if (!(current == begin && line == player)) {

        grid[current][line] = grid[current][line] + 1;

        seeds--;

        if (line == adversaire &&
            (grid[current][line] == 2 || grid[current][line] == 3)) {
          grid[current][line] = 0; // on capture les graines
        }
      }
    } else {

      current--;

      if (current == -1) {
        line = 1;
        current = 0;
      }

      if (!(current == begin && line == player)) {

        grid[current][line] = grid[current][line] + 1;

        seeds--;

        if (line == adversaire &&
            (grid[current][line] == 2 || grid[current][line] == 3)) {
          grid[current][line] = 0; // on capture les graines
        }
      }
    }
  }
}

void sow_seeds_update_grid_score(enum PlayerID player, struct Awale *awale,
                                 int target) {

  int seeds = awale->grid[target][player];

  enum PlayerID adversaire = next_player(awale);

  awale->grid[target][player] = 0;

  int begin = target;
  int current = begin;

  int line = player;

  while (seeds > 0) {

    if (line == 1) {

      current++;

      if (current == GRID_ROWS) {
        line = 0;
        current = 5;
      }

      // on vérifie si on ne tombe sur la case de départ pour semer une graine
      if (!(current == begin && line == player)) {

        awale->grid[current][line] = awale->grid[current][line] + 1;

        seeds--;

        // Si on est chez l'adversaire et qu'on a semé 2 ou 3 graines on les
        if (line == adversaire && (awale->grid[current][line] == 2 ||
                                   awale->grid[current][line] == 3)) {
          // on met à jour le score
          awale->score[player] =
              awale->score[player] +
              awale->grid[current]
                         [line]; // ne pas oubleir la graine qu'on a semé
          awale->grid[current][line] = 0; // on capture les graines
        }
      }
    } else {

      current--;

      if (current == -1) {
        line = 1;
        current = 0;
      }

      if (!(current == begin && line == player)) {

        awale->grid[current][line] = awale->grid[current][line] + 1;

        seeds--;

        if (line == adversaire && (awale->grid[current][line] == 2 ||
                                   awale->grid[current][line] == 3)) {
          // on met à jour le score
          awale->score[player] =
              awale->score[player] + awale->grid[current][line];
          awale->grid[current][line] = 0; // on capture les graines
        }
      }
    }
  }
}

enum CoupValidity is_coup_valid(struct Awale *awale, int target) {

  enum PlayerID nex_player = next_player(awale);

  if (target < 0 || target >= GRID_ROWS) {
    return INVALID_TARGET_OUT_OF_BOUNDS;
  }

  if (awale->grid[target][awale->current] == 0) {
    return INVALID_NO_SEEDS_IN_CASE;
  }

  int copy[GRID_ROWS][GRID_COLS];

  copy_grid(awale, copy);

  sow_seeds(awale->current, copy, target);

  const int famine = !player_has_seeds(nex_player, copy);

  if (famine) {
    return INVALID_OPONENT_HAS_NO_SEEDS;
  }

  return VALID;
}

enum CoupValidity play(struct Awale *awale, enum PlayerID player, int target) {

  if (awale->current == player) {
  } else {
    return INVALID_NOT_PLAYER_ROUND;
  }

  const enum CoupValidity validity = is_coup_valid(awale, target);

  if (validity == VALID) {

    sow_seeds_update_grid_score(player, awale, target);

    awale->current = next_player(awale);
  }

  return validity;
}

int can_play(const struct Awale *awale, enum PlayerID player) {

  for (int i = 0; i < GRID_ROWS; ++i) {
    int seeds = awale->grid[i][player];

    if (player == PLAYER1) {
      if (seeds >= i + 1) {
        return 1;
      }
    } else {
      if (seeds >= i - 1) {
        return 1;
      }
    }
  }

  return 0;
}

enum GameStatus status(const struct Awale *awale) {

  if (awale->score[PLAYER1] >= MAX_SCORE) {
    return GAME_OVER_PLAYER1_WINS;
  }

  if (awale->score[PLAYER2] >= MAX_SCORE) {
    return GAME_OVER_PLAYER2_WINS;
  }

  if (!player_has_seeds(awale->current, awale->grid) &&
      can_play(awale, next_player(awale))) {
    return PASS_TURN_NO_SEEDS;
  }

  if (!player_has_seeds(next_player(awale), awale->grid) &&
      !can_play(awale, awale->current)) {
    return GAME_OVER_STALEMATE;
  }

  return GAME_NOT_OVER;
}

enum PlayerID next_player(const struct Awale *awale) {
  return (awale->current + 1) % 2;
}

int winner(const struct Awale *awale, enum PlayerID *player) {

  if (awale->score[PLAYER1] == awale->score[PLAYER2]) {
    return 0;
  }

  *player = (awale->score[PLAYER1] > awale->score[PLAYER2]) ? PLAYER1 : PLAYER2;

  return 1;
}

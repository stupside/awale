#include "awale.h"

void initScore(struct Score *score) {
    score->score[0] = 0;
    score->score[1] = 0;
}

void initGrid(struct Grid *  grille) {
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            grille->grid[i][j] = 4;
        }
    }
};

int playerHasSeeds(struct Grid *  grille, int player) {
    for (int i = 0; i < GRID_ROWS; i++) {
        if (grille->grid[i][player] > 0) {
            return 1;
        }
    }
    return 0;
}

void deepCopyGrid(struct Grid *  grille, struct Grid *  grilleCopy) {
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            grilleCopy->grid[i][j] = grille->grid[i][j];
        }
    }
}

void calculeScoreAndUpdateGrid(struct Grid *  grille, enum PlayerID player, struct Score *  score) {
    enum PlayerID adversaire = player == PLAYER1 ? PLAYER2 : PLAYER1;
    for (int i = 0; i < GRID_ROWS; i++) {
        int graines = grille->grid[i][adversaire];
        if (graines == 2 || graines == 3) {
            grille->grid[i][adversaire] = 0;
            score->score[player] = score->score[player] + graines;
        }
    }
}

void sowSeeds(struct Grid *  grille, int player, int caseDepart) {
    // on récupère les graines de la case de départ
    int nbGraines = grille->grid[caseDepart][player];
    // on vide la case de départ
    grille->grid[caseDepart][player] = 0;
    int caseInitiale = caseDepart;
    int ligne = player;
    while (nbGraines > 0) {
        if (ligne == 1) {
            caseDepart++;
            if (caseDepart == GRID_ROWS) {
                caseDepart = 5;
                ligne = 0;
            }
            // on vérifie si on ne tombe sur la case de départ pour semer une graine
            if (!(caseDepart == caseInitiale && ligne == player)) {
                grille->grid[caseDepart][ligne] = grille->grid[caseDepart][ligne] + 1;
                nbGraines--;
            }
        } else {
            caseDepart--;
            if (caseDepart == -1) {
                caseDepart = 0;
                ligne = 1;
            }
            if (!(caseDepart == caseInitiale && ligne == player)) {
                grille->grid[caseDepart][ligne] = grille->grid[caseDepart][ligne] + 1;
                nbGraines--;
            }
        }
    }
}

enum CoupValidity coupIsValid(struct Grid *  grille, int caseDepart, int player) {

    // On vérifie que la case de départ contient des graines
    int adversaire = player == 0 ? 1 : 0;
    if (grille->grid[caseDepart][player] == 0) {
        return INVALID_NO_SEEDS_IN_CASE;
    }

    // On vérifie que le coup sème des graines chez l'adversaire si ce dernier n'a pas de graines sinon le coup n'est pas valide
    struct Grid grilleCopy;
    deepCopyGrid(grille, &grilleCopy);
    sowSeeds(&grilleCopy, player, caseDepart);

    if (!playerHasSeeds(grille, adversaire) && !playerHasSeeds(&grilleCopy, adversaire)) {
        return INVALID_OPONENT_HAS_NO_SEEDS;
    }

    // Sinon le coup est valide
    return VALID;
}


enum CoupValidity playCoup(enum PlayerID player, struct Grid *  grille, struct Score *  score, int caseDepart) {

    // Une fois que la case de départ est choisie on vérifie si le coup est valide
    enum CoupValidity coupValidity = coupIsValid(grille, caseDepart, player);
    if (coupValidity == VALID) {
        sowSeeds(grille, player, caseDepart);
        calculeScoreAndUpdateGrid(grille, player, score);
    }

    return coupValidity;
}

enum GameStatus checkGameStatus(struct Grid *  grille, struct Score *  score, enum PlayerID P) {
    if (!playerHasSeeds(grille, P)) {
        return PASS_TURN_NO_SEEDS;
    }
    if (score->score[PLAYER1] >= MAX_SCORE) {
        return GAME_OVER_PLAYER1_WINS;
    }
    if (score->score[PLAYER2] >= MAX_SCORE) {
        return GAME_OVER_PLAYER2_WINS;
    }
    return GAME_NOT_OVER;
}

enum PlayerID switchPlayer(enum PlayerID player) {
    return player == PLAYER1 ? PLAYER2 : PLAYER1;
}
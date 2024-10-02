#include <stdio.h>
#include "awale.h"

// http://archi02.free.fr/_pages/awele.htm
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

void calculeScoreAndUpdateGrid(struct Grid *  grille, int player, struct Score *  score) {
    int adversaire = player == 0 ? 1 : 0;
    for (int i = 0; i < GRID_ROWS; i++) {
        int graines = grille->grid[i][adversaire];
        if (graines == 2 || graines == 3) {
            grille->grid[i][adversaire] = 0;
            score->score[player] = score->score[player] + graines;
        }
    }
}

void sowSeeds(struct Grid *  grille, int player, int caseDepart) {
    int nbGraines = grille->grid[caseDepart][player];
    int caseInitiale = caseDepart;
    int ligne = player;
    while (nbGraines > 0) {
        if (ligne == 1) {
            caseDepart++;
            if (caseDepart == GRID_ROWS) {
                caseDepart = 0;
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
                caseDepart = GRID_ROWS - 1;
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


enum CoupValidity playCoup(enum PlayerID player, struct Grid *  grille, struct Score *  score) {
    // On demande à l'utilisateur de choisir une case de départ
    printf("Choisissez une case de départ le nombre doit être compris entre 0 et 5 : ");
    int caseDepart;
    while (1) {
        if (scanf("%d", &caseDepart) == 1 && caseDepart >= 0 && caseDepart <= 5) {
            break;
        } else {
            printf("Entrée invalide. Veuillez choisir un nombre entre 0 et 5 : ");
            while (getchar() != '\n'); // vide le buffer
        }
    }
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

void displayGrid(struct Grid *  grille) {
    for (int i = 0; i < GRID_ROWS; i++) {
        printf("%d ", grille->grid[i][0]);
    }
    printf("\n");
    for (int i = 0; i < GRID_ROWS; i++) {
        printf("%d ", grille->grid[i][1]);
    }
    printf("\n");
}

int test() {
    // Initialisation des variables
    enum CoupValidity coupValidity;

    // Initialisation de la grille
    struct Grid grid;
    initGrid(&grid);

    // Initialisation du score
    struct Score score;
    initScore(&score);

    // Boucle de jeu
    enum PlayerID currentPlayer = PLAYER1;
    while (1) {
        // on check que le jeu peut être pousuivi
        enum GameStatus gameStatus = checkGameStatus(&grid, &score, currentPlayer);

        if (gameStatus == GAME_OVER_PLAYER1_WINS) {
            printf("Le joueur 1 a gagné\n");
            break;
        }
        if (gameStatus == GAME_OVER_PLAYER2_WINS) {
            printf("Le joueur 2 a gagné\n");
            break;
        }
        if (gameStatus == PASS_TURN_NO_SEEDS) {
            printf("Le joueur a besoin de graines pour continuer à jouer\n");
            currentPlayer = switchPlayer(currentPlayer);
            continue;
        }

        // On affiche la grille
        displayGrid(&grid);
        coupValidity = INVALID;

        while (coupValidity != VALID) {
            // On demande au joueur de jouer
            coupValidity = playCoup(currentPlayer, &grid, &score);

            // On vérifie si le coup est valide
            if (coupValidity == INVALID_NO_SEEDS_IN_CASE) {
                printf("La case de départ ne contient pas de graines\n");
            }
            if (coupValidity == INVALID_OPONENT_HAS_NO_SEEDS) {
                printf("L'adversaire n'a pas de graines le coup doit permettre de lui en donner\n");
            }
        }

        // On passe au joueur suivant
        currentPlayer = switchPlayer(currentPlayer);
    }
    return 0;
}
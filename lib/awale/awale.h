#ifndef AWALE_H
#define AWALE_H

#define GRID_ROWS 6
#define GRID_COLS 2
#define MAX_SCORE 25

// La définition de ce cette version du jeu Awale est basée sur les règles expliquées sur le site suivante:
// http://archi02.free.fr/_pages/awele.html
// Dans cette version les règles principales sont les suivantes:

struct Grid {
    int grid[6][2];
};

enum CoupValidity {
    INVALID_NO_SEEDS_IN_CASE,
    INVALID_OPONENT_HAS_NO_SEEDS,
    VALID,
    INVALID
};

enum GameStatus {
    PASS_TURN_NO_SEEDS,
    GAME_OVER_PLAYER1_WINS, // +25 graines
    GAME_OVER_PLAYER2_WINS,
    GAME_NOT_OVER,
};

struct Score {
    int score[2];
};

struct Awale {
    struct Grid grid;
    struct Score score;
};

enum PlayerID {
    PLAYER1,
    PLAYER2
};

int test();

void initGrid(struct Grid *grille);

void initScore(struct Score *score);

// Retourne 1 si le joueur a des graines, 0 sinon
int playerHasSeeds(struct Grid *grille, int player);

void deepCopyGrid(struct Grid *grille, struct Grid *grilleCopy);

// Calcule le score et met à jour la grille selon les règles après le coup d'un joueur
void calculeScoreAndUpdateGrid(struct Grid *grille, enum PlayerID player, struct Score *score);

// Sème les graines à partir de la case de départ
void sowSeeds(struct Grid *grille, int player, int caseDepart);

// Vérifie si le coup est valide : la case de départ contient des graines et le coup sème des graines chez l'adversaire si celui-ci n'en a pas
enum CoupValidity coupIsValid(struct Grid *grille, int caseDepart, int player);

// Demande au joueur de jouer et vérifie si le coup est valide
enum CoupValidity playCoup(enum PlayerID player, struct Grid *grille, struct Score *score);

// Vérifie si le statut du jeu
enum GameStatus checkGameStatus(struct Grid *grille, struct Score *score, enum PlayerID P);

// Echange le joueur courant avec l'autre joueur
enum PlayerID switchPlayer(enum PlayerID player);

// Affiche la grille
void displayGrid(struct Grid *grille);

#endif // AWALE_H
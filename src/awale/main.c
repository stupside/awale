#include "lib/awale/awale.h"
#include <stdio.h>

int main() {
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

        // On affiche la grille
        printf("\n  Player 0\n");
        
        printf("  ");
        for (int i = 0; i < GRID_ROWS; i++) {
            printf("  [%2d] ", grid.grid[i][0]);
        }
        printf("\n");

        printf("  ");
        for (int i = 0; i < GRID_ROWS; i++) {
            printf("-------");
        }
        printf("\n");

        printf("  ");
        for (int i = 0; i < GRID_ROWS; i++) {
            printf("  [%2d] ", grid.grid[i][1]);
        }
        printf("\n  Player 1\n\n");
        // fin affichage grille

        coupValidity = INVALID;

        // On affiche le joueur qui doit jouer
        printf("C'est au joueur %d\nScore : %d\n", currentPlayer, score.score[currentPlayer]);

        while (coupValidity != VALID) {
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
            coupValidity = playCoup(currentPlayer, &grid, &score, caseDepart);

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
        if (gameStatus == GAME_OVER_STALEMATE) {
            enum PlayerID winner = findWinnerIfStale(&score);
            printf("Plus de mouvements possibles le jeu est fini, le joueur %d a gagné avec un score de %d\n", winner, score.score[winner]);
            break;
        }
    }
    return 0;
}
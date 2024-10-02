#pragma once

#include "lib/socket/pool.h"

#include "lobby.h"
#include "player.h"

#include "lib/awale/awale.h"

#define MAX_LOBBIES MAX_CLIENTS / 2

struct Server {
  struct Lobby lobbies[MAX_LOBBIES];

  struct SocketPool pool;
} Server;

struct Server awale_server() {

  static struct Server server = {.pool = (struct SocketPool){
                                     .count = 0,
                                 }};

  return server;
}

void on_challenge(struct Lobby *lobby) {
  struct Player player1 = lobby->players[PLAYER1];
  struct Player player2 = lobby->players[PLAYER2];

  write_to_socket(player2.client->sock, "You have been challenged!\n");
  write_to_socket(player1.client->sock,
                  "You have challenged the other player!\n");
}

void on_handle_challenge(struct Lobby *lobby, int accept) {
  struct Player player1 = lobby->players[PLAYER1];
  struct Player player2 = lobby->players[PLAYER2];

  if (accept) {
    write_to_socket(player1.client->sock, "Challenge accepted!\n");
    write_to_socket(player2.client->sock, "Challenge accepted!\n");
  } else {
    write_to_socket(player1.client->sock, "Challenge rejected!\n");
    write_to_socket(player2.client->sock, "Challenge rejected!\n");
  }
}

struct Lobby *find_lobby(struct Server *server, SocketClient *client) {
  for (int i = 0; i < MAX_LOBBIES; i++) {
    struct Lobby *game = &server->lobbies[i];

    if (game->players[PLAYER1].client == client ||
        game->players[PLAYER2].client == client) {
      return game;
    }
  }

  return NULL;
}

int challenge(struct Server *server, SocketClient *c1, SocketClient *c2) {

  static unsigned int lobbies = 0;

  if (lobbies == MAX_LOBBIES) {
    return 0;
  }

  if (find_lobby(server, c1) == NULL && find_lobby(server, c2) == NULL) {
    // Both clients are not in a game
  } else {
    return 0;
  }

  struct Lobby lobby = {
      .state = WAITING,
      .players =
          {
              [PLAYER1] = {.client = c1},
              [PLAYER2] = {.client = c2},
          },
  };

  server->lobbies[lobbies++] = lobby;

  on_challenge(&lobby);

  return 1;
}

int handle_challenge(struct Server *server, SocketClient *client, int accept) {

  struct Lobby *lobby = find_lobby(server, client);

  if (lobby == NULL) {
    return 0;
  }

  if (lobby->players[PLAYER2].client == client) {
    if (accept) {
      lobby->state = PLAYING;
    } else {
      lobby->state = FINISHED;
    }

    on_handle_challenge(lobby, accept);

    return 1;
  }

  return 0;
}

#pragma endregion
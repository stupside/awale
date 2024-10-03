#pragma once

#include "lobby.h"

#define MAX_LOBBIES MAX_CLIENTS / 2

struct Server {
  struct Lobby lobbies[MAX_LOBBIES];

  SocketPool pool;
};

struct Server *awale_server();

struct Lobby *find_lobby(struct Server *server, const SocketClient *client);

int challenge(struct Server *server, const SocketClient *c1,
              const SocketClient *c2);

int handle_challenge(struct Server *server, const SocketClient *client,
                     int accept);

#pragma endregion
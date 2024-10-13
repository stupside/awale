#ifndef AWALE_SERVER_H
#define AWALE_SERVER_H

#include "lobby.h"

#include "lib/socket/pool.h"

#define MAX_LOBBIES MAX_CLIENTS / 2

struct Server {
  struct Lobby lobbies[MAX_LOBBIES];

  SocketPool pool;
};

struct Server *awale_server();

struct Lobby *find_lobby(struct Server *server, const SocketClient *client);

int awale_play(struct Server *server, const SocketClient *client, int target);

int challenge(struct Server *server, SocketClient *c1, SocketClient *c2);

int handle_challenge(struct Server *server, const SocketClient *client,
                     int accept);

#endif
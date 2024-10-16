#ifndef AWALE_SERVER_H
#define AWALE_SERVER_H

#include "lobby.h"

#include "lib/socket/pool.h"

#define MAX_LOBBIES MAX_CLIENTS / 2

struct Server {
  SocketPool pool;

  struct Lobby *lobbies;
  unsigned int lobbies_c;
};

struct Server *awale_server();

void init_awale_server(struct Server *server);

struct Lobby *find_running_lobby(struct Server *server,
                                 const SocketClient *client);

struct Lobby *find_waiting_lobby(struct Server *server,
                                 const SocketClient *challenger,
                                 const SocketClient *challenged);

int awale_play(struct Server *server, const SocketClient *client, int target);

int challenge(struct Server *server, SocketClient *c1, SocketClient *c2);

int handle_challenge(struct Server *server, const SocketClient *challenger,
                     const SocketClient *challenged, int accept);

unsigned int is_lobby_player(const struct Lobby *lobby, const SocketClient *p);

#endif
#ifndef POOL_H
#define POOL_H

#include <string.h>
#include <sys/select.h>

typedef int SOCKET;
#define MAX_SOCKET_NAME 100

typedef struct SocketClient {
  SOCKET sock;

  unsigned int id;
  char name[MAX_SOCKET_NAME];

} SocketClient;

#define MAX_CLIENTS 100

typedef struct SocketPool {

  int count;

  fd_set rdfs;

  struct SocketClient clients[MAX_CLIENTS];
} SocketPool;

void clear_clients(SocketPool *pool);

int add_client(SocketPool *pool, const char *name, SOCKET socket);

void remove_client(SocketPool *sockets, int to_remove);

SocketClient *find_client(SocketPool *pool, unsigned int id);

#endif
#ifndef POOL_H
#define POOL_H

typedef int SOCKET;
#define MAX_SOCKET_NAME 100

typedef struct SocketClient {
  int online;
  SOCKET sock;

  unsigned int id;
  char name[MAX_SOCKET_NAME];

} SocketClient;

#define MAX_CLIENTS 100

typedef struct SocketPool {
  int count;
  struct SocketClient clients[MAX_CLIENTS];
} SocketPool;

void clear_clients(SocketPool *pool);

int add_client(SocketPool *pool, const char *name, SOCKET socket);

void archive_client(SocketPool *sockets, int to_remove);

SocketClient *find_client_by_id(SocketPool *pool, unsigned int id);
SocketClient *find_client_by_name(SocketPool *pool, const char *name);

#endif
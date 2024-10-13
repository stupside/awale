#ifndef POOL_H
#define POOL_H

typedef int SOCKET;
#define MAX_SOCKET_NAME 100

typedef struct SocketClient {
  int online;
  SOCKET socket;

  unsigned int id;
  char name[MAX_SOCKET_NAME];

} SocketClient;

#define MAX_CLIENTS 100

typedef struct SocketPool {
  int count;

  struct SocketClient clients[MAX_CLIENTS]; // TODO: make it dynamic
} SocketPool;

void clear_clients(SocketPool *pool);

int add_client(SocketPool *pool, const char *name, SOCKET socket);

unsigned int archive_client(SocketPool *sockets, int client_id);
unsigned int unarchive_client(SocketPool *sockets, SOCKET socket,
                              const char *name);

SocketClient *find_client_by_id(SocketPool *pool, unsigned int id);
SocketClient *find_client_by_name(SocketPool *pool, const char *name);

#endif
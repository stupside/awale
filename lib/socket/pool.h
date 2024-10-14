#ifndef POOL_H
#define POOL_H

#include "lib/valuable/limit.h"
#include "lib/valuable/type.h"

typedef int SOCKET;

typedef struct SocketClient {
  SOCKET socket;
  unsigned int online;

  unsigned int id;
  char name[USER_NAME_LEN];
  char password[USER_PASSWORD_LEN];

} SocketClient;

#define MAX_CLIENTS 100

typedef struct SocketPool {
  unsigned int count;
  struct SocketClient clients[MAX_CLIENTS]; // TODO: make it dynamic
} SocketPool;

void clear_clients(SocketPool *pool);

int add_client(SocketPool *pool, const char *name, const char *password,
               SOCKET socket, ClientId *client_id);

int archive_client(SocketPool *sockets, unsigned int client_id);
int unarchive_client(SocketPool *sockets, SOCKET socket, const char *name,
                     ClientId *client_id);

SocketClient *find_client_by_id(SocketPool *pool, ClientId client_id);
SocketClient *find_client_by_name(SocketPool *pool, const char *name);

#endif
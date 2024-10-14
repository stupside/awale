#ifndef POOL_H
#define POOL_H

#define MAX_SOCKET_NAME 100

typedef int SOCKET;

#define MAX_PASSWORD_SIZE 50

typedef struct SocketClient {
  int online;
  SOCKET socket;

  unsigned int id;
  char name[MAX_SOCKET_NAME];
  char password[MAX_PASSWORD_SIZE];

} SocketClient;

#define MAX_CLIENTS 100

typedef struct SocketPool {
  int count;

  struct SocketClient clients[MAX_CLIENTS]; // TODO: make it dynamic
} SocketPool;

void clear_clients(SocketPool *pool);

int add_client(SocketPool *pool, const char *name, const char *password,
               SOCKET socket, unsigned int *client_id);

int archive_client(SocketPool *sockets, unsigned int client_id);
int unarchive_client(SocketPool *sockets, SOCKET socket, const char *name,
                     unsigned int *client_id);

SocketClient *find_client_by_id(SocketPool *pool, unsigned int id);
SocketClient *find_client_by_name(SocketPool *pool, const char *name);

#endif
#pragma once

#include "socket.h"

#define MAX_CLIENTS 100

struct SocketPool {

  int count;

  fd_set rdfs;

  SocketClient list[MAX_CLIENTS];
} SocketPool;

void on_remove_client(const struct SocketPool *pool, const SocketClient *client,
                      unsigned int id) {

  char buffer[BUF_SIZE];

  strncpy(buffer, client->name, MAX_SOCKET_NAME - 1);
  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);

  write_to_sockets(pool->list, client, pool->count, buffer, 1);
}

void on_add_client(const struct SocketPool *pool, const SocketClient *client) {

  char buffer[BUF_SIZE];

  strncpy(buffer, client->name, BUF_SIZE - 1);
  strncat(buffer, " connected !", BUF_SIZE - strlen(buffer) - 1);

  write_to_sockets(pool->list, client, pool->count, buffer, 1);
}

void clear_clients(struct SocketPool *pool) {

  for (int i = 0; i < pool->count; i++) {

    const SocketClient *client = &pool->list[i];

    close_socket(client->sock);
  }
}

int add_client(struct SocketPool *pool, const char *name, SOCKET socket) {

  if (pool->count == MAX_CLIENTS) {
    return 0;
  }

  /* we add the client in the array */
  pool->list[pool->count] = (SocketClient){.sock = socket};

  strncpy(pool->list[pool->count].name, name, strlen(name));

  /* number client + 1 */
  pool->count++;

  on_add_client(pool, &pool->list[pool->count]);

  return 1;
}

void remove_client(struct SocketPool *sockets, int to_remove) {

  SocketClient *client = &sockets->list[to_remove];

  close_socket(client->sock);

  /* we remove the client in the array */
  memmove(sockets->list + to_remove, sockets->list + to_remove + 1,
          (sockets->count - to_remove - 1) * sizeof(SocketClient));

  /* number client - 1 */
  sockets->count--;

  on_remove_client(sockets, client, to_remove);
}

#pragma endregion
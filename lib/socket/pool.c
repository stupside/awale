#include "pool.h"

#include "socket.h"
#include <stdio.h>

void clear_clients(SocketPool *pool) {

  for (int i = 0; i < pool->count; i++) {

    const SocketClient *client = &pool->clients[i];

    close_socket(client->sock);
  }
}

int add_client(SocketPool *pool, const char *name, SOCKET socket) {

  static unsigned int id = 0;

  if (pool->count == MAX_CLIENTS) {
    return 0;
  }

  pool->clients[pool->count] = (SocketClient){.id = id++, .sock = socket};

  strncpy(pool->clients[pool->count].name, name, strlen(name));

  printf("Client %d added\n", pool->clients[pool->count].id);

  pool->count++;

  return 1;
}

void remove_client(SocketPool *sockets, int to_remove) {

  SocketClient *client = &sockets->clients[to_remove];

  close_socket(client->sock);

  memmove(sockets->clients + to_remove, sockets->clients + to_remove + 1,
          (sockets->count - to_remove - 1) * sizeof(SocketClient));

  printf("Client %d removed\n", client->id);

  sockets->count--;
}

SocketClient *find_client(SocketPool *pool, unsigned int id) {

  for (int i = 0; i < pool->count; i++) {

    SocketClient *client = &pool->clients[i];

    if (client->id == id) {
      return client;
    }
  }

  return NULL;
}
#include "pool.h"

#include "socket.h"

#include <stdio.h>
#include <string.h>

void clear_clients(SocketPool *pool) {

  for (int i = 0; i < pool->count; i++) {

    const SocketClient *client = &pool->clients[i];

    close_socket(client->sock);
  }
}

int add_client(SocketPool *pool, const char *name, SOCKET socket) {

  static unsigned int id = 0;

  SocketClient *existing = find_client_by_name(pool, name);

  if (existing == NULL) {

    if (existing->online) {
      printf("Client %d already connected\n", existing->id);
      return 0;
    }

    printf("Client %d added\n", pool->clients[pool->count].id);

    pool->clients[pool->count] =
        (SocketClient){.id = id++, .sock = socket, .online = 1};

    strncpy(pool->clients[pool->count].name, name, strlen(name));

    pool->count++;

  } else {
    printf("Client %d reconnected\n", existing->id);

    existing->online = 1;
    existing->sock = socket;
  }

  return 1;
}

void archive_client(SocketPool *sockets, int client_id) {

  SocketClient *client = &sockets->clients[client_id];

  client->online = 0;

  close_socket(client->sock);

  printf("Client %d archived\n", client->id);

  sockets->count--;
}

SocketClient *find_client_by_id(SocketPool *pool, unsigned int id) {

  for (int i = 0; i < pool->count; i++) {

    SocketClient *client = &pool->clients[i];

    if (client->id == id) {
      return client;
    }
  }

  return NULL;
}

SocketClient *find_client_by_name(SocketPool *pool, const char *name) {

  for (int i = 0; i < pool->count; i++) {

    SocketClient *client = &pool->clients[i];

    if (strcmp(client->name, name) == 0) {
      return client;
    }
  }

  return NULL;
}
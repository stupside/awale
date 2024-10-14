#include "pool.h"

#include "socket.h"

#include <stdio.h>
#include <string.h>

void clear_clients(SocketPool *pool) {

  for (int i = 0; i < pool->count; i++) {

    const SocketClient *client = &pool->clients[i];

    close_socket(client->socket);
  }
}

int add_client(SocketPool *pool, const char *name, const char *password,
               SOCKET socket, unsigned int *client_id) {

  if (pool->count == MAX_CLIENTS) {
    perror("Max clients reached");
    return 0;
  }

  if (find_client_by_name(pool, name)) {
    perror("Client already exists");
    return 0;
  }

  const unsigned int id = pool->count++;

  pool->clients[id] = (SocketClient){.id = id, .socket = socket, .online = 1};

  struct SocketClient *client = &pool->clients[id];

  strncpy(client->name, name, strlen(name));
  strncpy(client->password, password, strlen(password));

  printf("Client %s added with id %d\n", client->name, client->id);

  *client_id = client->id;

  return 1;
}

int archive_client(SocketPool *sockets, unsigned int client_id) {

  SocketClient *client = &sockets->clients[client_id];

  if (!client) {
    return 0;
  }

  if (!client->online) {
    perror("Client already offline");
    return 0;
  }

  close_socket(client->socket);

  client->online = 0;
  client->socket = 0;

  printf("Client with id %d archived\n", client->id);

  return 1;
}

int unarchive_client(SocketPool *sockets, SOCKET socket, const char *name,
                     unsigned int *client_id) {

  SocketClient *client = find_client_by_name(sockets, name);

  if (!client) {
    perror("Client not found");
    return 0;
  }

  if (client->online) {
    perror("Client already online");
    return 0;
  }

  client->online = 1;
  client->socket = socket;

  printf("Client with id %d unarchived\n", client->id);

  *client_id = client->id;

  return 1;
}

SocketClient *find_client_by_id(SocketPool *pool, unsigned int id) {

  for (unsigned int i = 0; i < pool->count; i++) {

    SocketClient *client = &pool->clients[i];

    if (!client) {
      continue;
    }

    if (client->id == id) {
      return client;
    }
  }

  perror("Client not found");

  return NULL;
}

SocketClient *find_client_by_name(SocketPool *pool, const char *name) {

  for (int i = 0; i < pool->count; i++) {

    SocketClient *client = &pool->clients[i];

    if (strcmp(client->name, name) == 0) {
      return client;
    }
  }

  perror("Client not found");

  return NULL;
}
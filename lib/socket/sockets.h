#include "socket.h"

struct SocketClients {

  int count;

  fd_set rdfs;

  SocketClient list[MAX_CLIENTS];
} SocketClients;

struct SocketClients new_clients() {
  return (struct SocketClients){
      .count = 0,
  };
}

void clear_clients(struct SocketClients *clients) {

  for (int i = 0; i < clients->count; i++) {

    const SocketClient *client = &clients->list[i];

    close_socket(client->sock);
  }
}

int add_client(struct SocketClients *clients, SocketClient client) {

  if (clients->count == MAX_CLIENTS) {
    return 0;
  }

  /* we add the client in the array */
  clients->list[clients->count] = client;

  /* number client + 1 */
  clients->count++;

  return 1;
}

void remove_client(struct SocketClients *clients, int to_remove) {

  SocketClient *client = &clients->list[to_remove];

  close_socket(client->sock);

  /* we remove the client in the array */
  memmove(clients->list + to_remove, clients->list + to_remove + 1,
          (clients->count - to_remove - 1) * sizeof(SocketClient));

  /* number client - 1 */
  clients->count--;
}
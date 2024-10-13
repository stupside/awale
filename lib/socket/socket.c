#include "socket.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void close_socket(SOCKET sock) { close(sock); }

int write_to_socket(SOCKET sock, const char *buffer) {

  return send(sock, buffer, strlen(buffer), 0) > 0;
}

int write_to_sockets(const SocketPool *pool, const SocketClient *sender,
                     const char *buffer) {

  int ok = 0;

  for (int i = 0; i < pool->count; i++) {

    if (sender->id == pool->clients[i].id) {
      continue;
    }

    if (write_to_socket(pool->clients[i].socket, buffer)) {
      ++ok;
    }
  }

  return ok;
}

int read_from_socket(SOCKET sock, char *buffer) {
  int n = 0;

  if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
    return 0;
  }

  buffer[n] = 0;

  return n;
}

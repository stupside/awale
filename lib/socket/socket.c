#include "socket.h"

#include "lib/cmds/cmd.h"

#include <stdio.h>
#include <stdlib.h>
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

    const SocketClient *client = &pool->clients[i];

    if (!client) {
      continue;
    }

    if (client->id == sender->id) {
      continue;
    }

    if (write_to_socket(client->socket, buffer)) {
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

int send_cmd_to(SOCKET sock, enum CMD cmd_id, const void *data,
                unsigned int data_size) {

  char *cmd = inline_cmd(cmd_id, data, data_size);

  int ok = write_to_socket(sock, cmd);

  free(cmd);

  return ok;
}

int send_cmd_to_all(const SocketPool *pool, const SocketClient *sender,
                    enum CMD cmd_id, const void *data, unsigned int data_size) {
  char *cmd = inline_cmd(cmd_id, data, data_size);

  int ok = write_to_sockets(pool, sender, cmd);

  free(cmd);

  return ok;
}

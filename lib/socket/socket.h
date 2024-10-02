#pragma once

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 1977

#define CRLF "\r\n"

typedef int SOCKET;

typedef struct in_addr IN_ADDR;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;

#define BUF_SIZE 1024
#define MAX_SOCKET_NAME 100

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

typedef struct {
  SOCKET sock;

  char name[MAX_SOCKET_NAME];
} SocketClient;

void close_socket(SOCKET sock) { close(sock); }

int write_to_socket(SOCKET sock, const char *buffer) {
  int ok = send(sock, buffer, strlen(buffer), 0) < 0;

  if (ok) {
    perror("send()");
  }

  return ok;
}

void write_to_sockets(const SocketClient *clients, const SocketClient *sender,
                      int count, const char *buffer, char from_server) {

  char message[BUF_SIZE];

  message[0] = 0;

  for (int i = 0; i < count; i++) {

    /* we don't send message to the sender */
    if (sender->sock != clients[i].sock) {
      if (from_server == 0) {

        strncpy(message, sender->name, BUF_SIZE - 1);
        strncat(message, " : ", sizeof message - strlen(message) - 1);
      }

      strncat(message, buffer, sizeof message - strlen(message) - 1);

      write_to_socket(clients[i].sock, message);
    }
  }
}

int read_from_socket(SOCKET sock, char *buffer) {
  int n = 0;

  if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
    perror("recv()");
  }

  buffer[n] = 0;

  return n;
}

#pragma endregion
#include "main.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "lib/server/server.h"

#include "lib/socket/socket.h"

int init(unsigned int port) {

  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

  SOCKADDR_IN sin = {0};

  if (sock == INVALID_SOCKET) {
    exit(errno);
  }

  int opt = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    close_socket(sock);
    exit(errno);
  }

  sin.sin_port = htons(port);
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR) {
    exit(errno);
  }

  if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR) {
    exit(errno);
  }

  return sock;
}

void app(unsigned int port, struct ServerMediator *mediator) {
  SOCKET sock = init(port);

  char buffer[BUF_SIZE];

  int maxfd = sock;

  struct Server *server = awale_server();

  init_awale_server(server);

  fd_set rdfs;

  while (1) {

    FD_ZERO(&rdfs);

    /* add STDIN_FILENO */
    FD_SET(STDIN_FILENO, &rdfs);

    /* add the connection socket */
    FD_SET(sock, &rdfs);

    /* add socket of each client */
    for (int i = 0; i < server->pool.count; i++) {
      FD_SET(server->pool.clients[i].socket, &rdfs);
    }

    if (select(maxfd + 1, &rdfs, NULL, NULL, NULL) == -1) {
      exit(errno);
    }

    /* something from standard input : i.e keyboard */
    if (FD_ISSET(STDIN_FILENO, &rdfs)) {
      break;
    } else if (FD_ISSET(sock, &rdfs)) {

      SOCKADDR sock_addr = {0};

      unsigned int sock_addr_size = sizeof sock_addr;

      int csock = accept(sock, &sock_addr, &sock_addr_size);

      if (csock == SOCKET_ERROR) {
        continue;
      }

      if (read_from_socket(csock, buffer) == -1) {
        continue;
      }

      {
        handle_cmd(mediator, csock, buffer, PERSIST);

        if (read_from_socket(csock, buffer) == -1) {
          continue;
        }

        if (handle_cmd(mediator, csock, buffer, NO_PERSIST)) {
          FD_SET(csock, &rdfs);
          maxfd = csock > maxfd ? csock : maxfd;
        } else {
          close_socket(csock);
        }
      }

    } else {
      for (unsigned int idx = 0; idx < server->pool.count; idx++) {

        const struct SocketClient *client = &server->pool.clients[idx];

        /* a client is talking */
        if (FD_ISSET(client->socket, &rdfs)) {

          if (read_from_socket(client->socket, buffer)) {

            if (handle_cmd(mediator, client->id, buffer, PERSIST)) {
              // Command handled
            } else {
              perror("Failed to compute command");
            }
          } else {
            const int ok = archive_client(&server->pool, idx);

            if (ok) {
              printf("Client archived\n");
            } else {
              perror("Failed to archive client");
            }
          }
          break;
        }
      }
    }
  }

  clear_clients(&server->pool);

  close_socket(sock);
}
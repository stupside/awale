#include "main.h"

#include <arpa/inet.h>

#include "lib/server/server.h"
#include "lib/socket/cmd.h"
#include "lib/socket/pool.h"
#include "lib/socket/socket.h"

int init(void) {

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

  sin.sin_port = htons(PORT);
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

void app(struct Mediator *mediator) {
  SOCKET sock = init();

  char buffer[BUF_SIZE];

  int maxfd = sock;

  /* an array for all clients */
  struct Server server = awale_server();

  while (1) {

    FD_ZERO(&server.pool.rdfs);

    /* add STDIN_FILENO */
    FD_SET(STDIN_FILENO, &server.pool.rdfs);

    /* add the connection socket */
    FD_SET(sock, &server.pool.rdfs);

    /* add socket of each client */
    for (int i = 0; i < server.pool.count; i++) {
      FD_SET(server.pool.list[i].sock, &server.pool.rdfs);
    }

    if (select(maxfd + 1, &server.pool.rdfs, NULL, NULL, NULL) == -1) {
      exit(errno);
    }

    /* something from standard input : i.e keyboard */
    if (FD_ISSET(STDIN_FILENO, &server.pool.rdfs)) {
      break;
    } else if (FD_ISSET(sock, &server.pool.rdfs)) {

      SOCKADDR sock_addr = {0};

      unsigned int sock_addr_size = sizeof sock_addr;

      int csock = accept(sock, &sock_addr, &sock_addr_size);

      if (csock == SOCKET_ERROR) {
        continue;
      }

      if (read_from_socket(csock, buffer) == -1) {
        continue;
      }

      maxfd = csock > maxfd ? csock : maxfd;

      FD_SET(csock, &server.pool.rdfs);

      add_client(&server.pool, buffer, csock);

    } else {
      int i = 0;

      for (i = 0; i < server.pool.count; i++) {

        SOCKET socket = server.pool.list[i].sock;

        /* a client is talking */
        if (FD_ISSET(socket, &server.pool.rdfs)) {

          if (read_from_socket(socket, buffer)) {
            compute_cmd(mediator, buffer);
          } else {
            remove_client(&server.pool, i);
          }
          break;
        }
      }
    }
  }

  clear_clients(&server.pool);

  close_socket(sock);
}
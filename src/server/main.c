#include "main.h"

#include <arpa/inet.h>

#include "lib/socket/socket.h"
#include "lib/socket/sockets.h"

int init(void) {

  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

  SOCKADDR_IN sin = {0};

  if (sock == INVALID_SOCKET) {
    perror("socket()");
    exit(errno);
  }

  sin.sin_port = htons(PORT);
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock, (SOCKADDR *)&sin, sizeof sin) == SOCKET_ERROR) {
    perror("bind()");
    exit(errno);
  }

  if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR) {
    perror("listen()");
    exit(errno);
  }

  return sock;
}

void app(void) {
  SOCKET sock = init();

  char buffer[BUF_SIZE];

  int max = sock;

  /* an array for all clients */
  struct SocketClients clients = new_clients();

  while (1) {

    FD_ZERO(&clients.rdfs);

    /* add STDIN_FILENO */
    FD_SET(STDIN_FILENO, &clients.rdfs);

    /* add the connection socket */
    FD_SET(sock, &clients.rdfs);

    /* add socket of each client */
    for (int i = 0; i < clients.count; i++) {
      FD_SET(clients.list[i].sock, &clients.rdfs);
    }

    if (select(max + 1, &clients.rdfs, NULL, NULL, NULL) == -1) {
      perror("select()");
      exit(errno);
    }

    /* something from standard input : i.e keyboard */
    if (FD_ISSET(STDIN_FILENO, &clients.rdfs)) {
      /* stop process when type on keyboard */
      break;
    } else if (FD_ISSET(sock, &clients.rdfs)) {

      /* new client */
      SOCKADDR sock_addr = {0};

      unsigned int sock_addr_size = sizeof sock_addr;

      int csock = accept(sock, &sock_addr, &sock_addr_size);

      if (csock == SOCKET_ERROR) {
        perror("accept()");
        continue;
      }

      /* after connecting the client sends its name */
      if (read_from_socket(csock, buffer) == -1) {
        /* disconnected */
        continue;
      }

      /* what is the new maximum fd ? */
      max = csock > max ? csock : max;

      FD_SET(csock, &clients.rdfs);

      SocketClient client = {csock};

      strncpy(client.name, buffer, BUF_SIZE - 1);

      add_client(&clients, client);

    } else {
      int i = 0;

      for (i = 0; i < clients.count; i++) {

        /* a client is talking */
        if (FD_ISSET(clients.list[i].sock, &clients.rdfs)) {

          SocketClient client = clients.list[i];

          /* client disconnected */
          if (read_from_socket(clients.list[i].sock, buffer) == 0) {

            remove_client(&clients, clients.count);

            strncpy(buffer, client.name, BUF_SIZE - 1);
            strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);

            write_to_sockets(clients.list, client, clients.count, buffer, 1);
          } else {

            write_to_sockets(clients.list, client, clients.count, buffer, 0);
          }
          break;
        }
      }
    }
  }

  clear_clients(&clients);

  close_socket(sock);
}
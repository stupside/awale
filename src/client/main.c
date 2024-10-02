#include "main.h"

#include <netdb.h>

#include "lib/socket/socket.h"

int init(const char *address) {

  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

  SOCKADDR_IN sin = {0};

  struct hostent *hostinfo;

  if (sock == INVALID_SOCKET) {
    perror("socket()");
    exit(errno);
  }

  hostinfo = gethostbyname(address);

  if (hostinfo == NULL) {
    fprintf(stderr, "Unknown host %s.\n", address);
    exit(EXIT_FAILURE);
  }

  sin.sin_addr = *(IN_ADDR *)hostinfo->h_addr;
  sin.sin_port = htons(PORT);
  sin.sin_family = AF_INET;

  if (connect(sock, (SOCKADDR *)&sin, sizeof(SOCKADDR)) == SOCKET_ERROR) {
    perror("connect()");
    exit(errno);
  }

  return sock;
}

void app(const char *address, const char *name) {

  SOCKET sock = init(address);

  char buffer[BUF_SIZE];

  fd_set rdfs;

  /* send our name */
  write_to_socket(sock, name);

  while (1) {
    FD_ZERO(&rdfs);

    /* add STDIN_FILENO */
    FD_SET(STDIN_FILENO, &rdfs);

    /* add the socket */
    FD_SET(sock, &rdfs);

    if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1) {
      perror("select()");
      exit(errno);
    }

    /* something from standard input : i.e keyboard */
    if (FD_ISSET(STDIN_FILENO, &rdfs)) {

      fgets(buffer, BUF_SIZE - 1, stdin);
      {
        char *p = NULL;

        p = strstr(buffer, "\n");
        if (p != NULL) {

          *p = 0;
        } else {

          /* fclean */
          buffer[BUF_SIZE - 1] = 0;
        }
      }

      write_to_socket(sock, buffer);

    } else if (FD_ISSET(sock, &rdfs)) {

      /* server down */
      if (read_from_socket(sock, buffer) == 0) {
        printf("Server disconnected !\n");
        break;
      }

      puts(buffer);
    }
  }

  close_socket(sock);
}

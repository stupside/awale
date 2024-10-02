#include "main.h"

#include <netdb.h>

#include "lib/socket/cmd.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/user.h"

int init(const char *address) {

  SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

  SOCKADDR_IN sin = {0};

  struct hostent *hostinfo;

  if (sock == INVALID_SOCKET) {
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
    exit(errno);
  }

  return sock;
}

void app(const char *address, const char *name) {

  SOCKET sock = init(address);

  char buffer[BUF_SIZE];

  fd_set rdfs;

  write_to_socket(sock, name);

  {
    struct UserLoginReq req;

    strcpy(req.name, name);

    char *cmd = stringify_cmd(CMD_USER_LOGIN, &req, sizeof(req));

    write_to_socket(sock, cmd);

    free(cmd);
  }

  while (1) {
    FD_ZERO(&rdfs);

    /* add STDIN_FILENO */
    FD_SET(STDIN_FILENO, &rdfs);

    /* add the socket */
    FD_SET(sock, &rdfs);

    if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1) {
      exit(errno);
    }

    /* something from standard input : i.e keyboard */
    if (FD_ISSET(STDIN_FILENO, &rdfs)) {

      fgets(buffer, BUF_SIZE - 1, stdin);

      {
        struct ChatWriteReq data;

        strcpy(data.message, buffer);

        char *cmd = stringify_cmd(CMD_CHAT_WRITE, &data, sizeof(data));

        write_to_socket(sock, cmd);

        free(cmd);
      }

    } else if (FD_ISSET(sock, &rdfs)) {

      /* server down */
      if (read_from_socket(sock, buffer) == 0) {
        break;
      }

      puts(buffer);
    }
  }

  {
    char *cmd = stringify_cmd(CMD_USER_LOGOUT, NULL, 0);

    write_to_socket(sock, cmd);

    free(cmd);
  }

  close_socket(sock);
}

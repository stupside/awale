#ifndef CLIENT_CMD_H
#define CLIENT_CMD_H

#define MAX_CLIENT_COMMANDS 50

#include "lib/socket/socket.h"

struct ClientHandler {
  char prefix[15];
  unsigned int (*handle)(SOCKET sock, char *argv[], unsigned int argslen);
};

struct ClientMediator {
  unsigned int handler_c;
  struct ClientHandler handlers[MAX_CLIENT_COMMANDS];
};

unsigned int register_client_cmd(
    struct ClientMediator *mediator, char prefix[15],
    unsigned int (*callback)(SOCKET sock, char *argv[], unsigned int argslen));

unsigned int handle_client_cmd(SOCKET sock,
                               const struct ClientMediator *mediator,
                               char *input);

#endif
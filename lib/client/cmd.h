#ifndef CLIENT_CMD_H
#define CLIENT_CMD_H

#define MAX_PREFIX_LEN 25
#define MAX_CLIENT_COMMANDS 50

#include "lib/socket/socket.h"

struct ClientHandler {
  char prefix[MAX_PREFIX_LEN];
  unsigned int (*handle)(SOCKET sock, char *argv[], unsigned int argslen);
};

struct ClientMediator {
  unsigned int handler_c;
  struct ClientHandler handlers[MAX_CLIENT_COMMANDS];
};

unsigned int register_client_cmd(
    struct ClientMediator *mediator, const char prefix[MAX_PREFIX_LEN],
    unsigned int (*callback)(SOCKET sock, char *argv[], unsigned int argslen));

unsigned int handle_client_cmd(SOCKET sock,
                               const struct ClientMediator *mediator,
                               char *input);

#endif
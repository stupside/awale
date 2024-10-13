#ifndef CLIENT_CMD_H
#define CLIENT_CMD_H

#define MAX_COMMANDS 50
#define MAX_ARGS 5

#include "lib/socket/cmd.h"

enum CLIENT_CMD {
  CLIENT_CMD_CHAT_WRITE,
  // CLIENT_CMD_PLAY_MOVE
};

struct ClientHandler {
  char prefix[15];
  unsigned int (*handle)(SOCKET sock, char *argv[], unsigned int argslen);
};

struct ClientMediator {
  struct ClientHandler handlers[MAX_COMMANDS];
};

unsigned int register_client_cmd(
    struct ClientMediator *mediator, char prefix[15],
    unsigned int (*callback)(SOCKET sock, char *argv[], unsigned int argslen));

unsigned int handle_client_cmd(SOCKET sock,
                               const struct ClientMediator *mediator,
                               char *input);

#endif
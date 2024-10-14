#ifndef CMD_MEDIATOR_H
#define CMD_MEDIATOR_H

#define MAX_COMMANDS 25

#include "lib/valuable/type.h"

#include "cmd.h"

struct Handler {
  unsigned int (*handle)(unsigned int client_id, const void *data);
};

struct ServerMediator {
  struct Handler handlers[MAX_COMMANDS];
};

struct ServerMediator new_mediator();

void register_cmd(struct ServerMediator *mediator, CommandId cmd_id,
                  unsigned int (*callback)(ClientId client_id,
                                           const void *data));

int handle_cmd(const struct ServerMediator *dispatcher, unsigned int client_id,
               const char *cmd);

#endif
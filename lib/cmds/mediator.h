#ifndef CMD_MEDIATOR_H
#define CMD_MEDIATOR_H

#define MAX_COMMANDS 25

#include "lib/valuable/type.h"

#include "cmd.h"

struct Handler {
  unsigned int (*handle)(unsigned int client_id, const void *data);
};

struct Mediator {
  struct Handler handlers[MAX_COMMANDS];
};

struct Mediator new_mediator();

void register_cmd(struct Mediator *mediator, CommandId cmd_id,
                  unsigned int (*callback)(ClientId client_id,
                                           const void *data));

int handle_cmd(const struct Mediator *dispatcher, unsigned int client_id,
               const char *cmd);

#endif
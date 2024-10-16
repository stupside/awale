#ifndef CMD_MEDIATOR_H
#define CMD_MEDIATOR_H

#define MAX_COMMANDS 50

#include "lib/valuable/type.h"

#include "cmd.h"
#include "persistor.h"

enum Persist {
  NO_PERSIST,
  PERSIST,
};

struct Handler {
  enum Persist persist;

  unsigned int (*handle)(unsigned int client_id, const void *data);
};

struct ServerMediator {
  Persistor persistor;

  struct Handler handlers[MAX_COMMANDS];
};

void register_cmd(struct ServerMediator *mediator, CommandId cmd_id,
                  unsigned int (*callback)(ClientId client_id,
                                           const void *data),
                  enum Persist persist);

int handle_cmd(const struct ServerMediator *dispatcher, unsigned int client_id,
               const char *cmd, enum Persist persist);

#endif
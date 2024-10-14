#include "mediator.h"

#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>

struct ServerMediator new_mediator() {
  return (struct ServerMediator){.handlers = {0}};
}

void register_cmd(struct ServerMediator *mediator, CommandId cmd_id,
                  unsigned int (*callback)(ClientId client_id,
                                           const void *data)) {
  mediator->handlers[cmd_id] = (struct Handler){.handle = callback};
}

int handle_cmd(const struct ServerMediator *dispatcher, unsigned int client_id,
               const char *cmd) {

  CommandId cmd_id;

  unsigned char *cmd_payload = NULL;

  int ok = parse_cmd(cmd, &cmd_id, &cmd_payload);

  if (!ok) {
    return 0;
  }

  const struct Handler *handler = &dispatcher->handlers[cmd_id];

  if (!handler) {
    free(cmd_payload);

    perror("No handler for command");
    return 0;
  }

  if (handler->handle) {
    ok = handler->handle(client_id, cmd_payload);
  }

  free(cmd_payload);

  return ok;
}
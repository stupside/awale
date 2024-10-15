#ifndef ERROR_HANDLERS_H
#define ERROR_HANDLERS_H

#include "lib/cmds/mediator.h"
#include "lib/display/colors.h"
#include "lib/socket/cmd.h"

#include "lib/socket/cmds/error.h"
#include <stdio.h>

unsigned int on_error_event(unsigned int client_id, const void *data) {

  const struct ErrorEvent *event = data;

  PRINT_COLOR(COLOR_RED, "Error: %s\n", event->message);

  return 1;
};

void add_error_handlers(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_ERROR_EVENT, &on_error_event);
}

#endif
#ifndef ERROR_HANDLERS_H
#define ERROR_HANDLERS_H

#include "lib/cmds/mediator.h"

unsigned int on_error_event(unsigned int client_id, const void *data);

void add_error_handlers(struct ServerMediator *mediator);

#endif
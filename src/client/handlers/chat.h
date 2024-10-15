#ifndef CHAT_HANDLERS_H
#define CHAT_HANDLERS_H

#include "lib/cmds/mediator.h"

unsigned int on_chat_write(unsigned int client_id, const void *data);

void add_chat_cmds(struct ServerMediator *mediator);

#endif
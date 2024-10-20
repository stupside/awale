#ifndef USER_HANDLERS_H
#define USER_HANDLERS_H

#include "lib/cmds/mediator.h"

ClientId current_client_id();

void add_user_cmds(struct ServerMediator *mediator);

#endif
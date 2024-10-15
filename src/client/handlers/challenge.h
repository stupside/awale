#ifndef CHALLENGE_HANDLERS_H
#define CHALLENGE_HANDLERS_H

#include "lib/cmds/mediator.h"

unsigned int on_challenge(unsigned int client_id, const void *data);
unsigned int on_handle_challenge_event(unsigned int client_id,
                                       const void *data);

void add_challenge_cmds(struct ServerMediator *mediator);

#endif
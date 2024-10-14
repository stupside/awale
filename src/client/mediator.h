#ifndef MEDIATOR_H
#define MEDIATOR_H

#include "lib/cmds/mediator.h"

#include "lib/client/cmd.h"

void init_mediator(struct Mediator *mediator);

void init_client_mediator(struct ClientMediator *mediator);

#endif
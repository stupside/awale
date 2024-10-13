#ifndef MEDIATOR_H
#define MEDIATOR_H

#include "lib/socket/cmd.h"
#include "lib/client/cmd.h"

void init_mediator(struct Mediator *mediator);

void init_client_mediator(struct ClientMediator *mediator);

#endif
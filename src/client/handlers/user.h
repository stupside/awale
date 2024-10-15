#ifndef USER_HANDLERS_H
#define USER_HANDLERS_H

#include "lib/cmds/mediator.h"

static ClientId CLIENT_ID;

unsigned int on_user_login(unsigned int client_id, const void *data);

unsigned int on_user_login_event(unsigned int client_id, const void *data);

unsigned int on_user_logout(unsigned int client_id, const void *data);

unsigned int on_user_list(unsigned int client_id, const void *data);

unsigned int on_user_get_info(unsigned int client_id, const void *data);

void add_user_cmds(struct ServerMediator *mediator);

#endif
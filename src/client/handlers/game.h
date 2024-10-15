#ifndef GAME_HANDLERS_H
#define GAME_HANDLERS_H

#include "lib/cmds/mediator.h"

unsigned int on_game_play(unsigned int client_id, const void *data);

unsigned int on_game_leave(unsigned int client_id, const void *data);

unsigned int on_game_leave_event(unsigned int client_id, const void *data);

unsigned int on_game_state_event(unsigned int client_id, const void *data);

unsigned int on_game_state(unsigned int client_id, const void *data);

unsigned int on_game_observe(unsigned int client_id, const void *data);

unsigned int on_game_observe_event(unsigned int client_id, const void *data);

void add_game_cmds(struct ServerMediator *mediator);

#endif
#ifndef USER_HANDLERS_H
#define USER_HANDLERS_H

#include <stdio.h>

#include "lib/cmds/mediator.h"
#include "lib/socket/cmd.h"

#include "lib/socket/cmds/user.h"

unsigned int on_user_login(unsigned int client_id, const void *data) {

  const struct UserLoginEvent *event = data;

  printf("User %d logged in\n", event->id);

  return 1;
};

unsigned int on_user_logout(unsigned int client_id, const void *data) {

  const struct UserLogoutEvent *event = data;

  printf("User %d logged out\n", event->client_id);

  return 1;
};

unsigned int on_user_list(unsigned int client_id, const void *data) {

  const struct UserListRes *res = data;

  printf("Online users: %d\n", res->count);

  for (unsigned int i = 0; i < res->count; i++) {
    printf("User %d: %s\n", res->users[i].client_id, res->users[i].name);
  }

  return 1;
};

void add_user_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_USER_LIST_ALL, &on_user_list);

  register_cmd(mediator, CMD_USER_LOGIN_EVENT, &on_user_login);
  register_cmd(mediator, CMD_USER_LOGOUT_EVENT, &on_user_logout);
}

#endif
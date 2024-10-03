#include "mediator.h"

#include <stddef.h>
#include <stdio.h>

#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/user.h"

unsigned int on_user_login(unsigned int client_id, const void *data) {

  const struct UserLoginEvent *event = data;

  printf("User %d logged in\n", event->id);

  return 1;
};

unsigned int on_user_logout(unsigned int client_id, const void *data) {

  const struct UserLogoutEvent *event = data;

  printf("User %d logged out\n", event->id);

  return 1;
};

unsigned int on_chat_write(unsigned int client_id, const void *data) {

  const struct ChatWriteEvent *event = data;

  printf("User %d wrote: %s\n", event->client_id, event->message);

  return 1;
};

void init_mediator(struct Mediator *mediator) {

  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);
  register_cmd(mediator, CMD_USER_LOGOUT, &on_user_logout);

  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write);

  // register_cmd(mediator, CMD_CHALLENGE, NULL);

  // register_cmd(mediator, CMD_CHALLENGE_ACCEPT, NULL);
  // register_cmd(mediator, CMD_CHALLENGE_REJECT, NULL);
}

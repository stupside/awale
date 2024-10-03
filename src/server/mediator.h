#pragma once

#include "lib/server/server.h"
#include "lib/socket/cmd.h"

#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/user.h"

unsigned int on_user_login(const void *data) {
  const struct UserLoginReq *cmd = data;

  printf("UserLoginCmd: %s\n", cmd->name);

  return 0;
};

unsigned int on_chat_write(const void *data) {

  const struct ChatWriteReq *cmd = data;

  printf("ChatWriteCmd: %s\n", cmd->message);

  struct Server server = awale_server();

  printf("Broadcasting message to %d clients\n", server.pool.count);

  for (int i = 0; i < server.pool.count; i++) {
    write_to_socket(server.pool.list[i].sock, cmd->message);
  }

  return 0;
};

void init_mediator(struct Mediator *mediator) {
  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);
  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write);
}

#pragma endregion
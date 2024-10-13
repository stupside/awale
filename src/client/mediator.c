#include "mediator.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/game.h"
#include "lib/socket/cmds/user.h"
#include "lib/socket/socket.h"

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

unsigned int on_game_state(unsigned int client_id, const void *data) {

  const struct GameStateEvent *event = data;

  printf("Game state: %d\n", event->status);

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      printf("[%2d]", event->grid[i][j]);
    }
    printf("\n");
  }

  return 1;
};

void init_mediator(struct Mediator *mediator) {

  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);
  register_cmd(mediator, CMD_USER_LOGOUT, &on_user_logout);

  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write);

  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);
}

//__________________________________________________________________________________________________________________

unsigned int input_formatter_chat(SOCKET sock, char *argv[],
                                  unsigned int argslen) {
  struct ChatWriteReq req;
  char formatted_message[255];

  formatted_message[0] = '\0';

  for (unsigned int i = 1; i < argslen; i++) {
    strcat(formatted_message, argv[i]);
    if (i < argslen - 1) {
      strcat(formatted_message, " ");
    }
  }

  strncpy(req.message, formatted_message, sizeof(req.message) - 1);
  req.message[sizeof(req.message) - 1] = '\0';

  char *cmd = inline_cmd(CMD_CHAT_WRITE, &req, sizeof(struct ChatWriteReq));

  write_to_socket(sock, cmd);

  free(cmd);

  return 1;
}

void init_client_mediator(struct ClientMediator *mediator) {

  register_client_cmd(mediator, "/chat", &input_formatter_chat);
}
#include "mediator.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/error.h"
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

unsigned int on_challenge(unsigned int client_id, const void *data) {

  const struct ChallengeEvent *event = data;

  printf("You have been challenged by %d, tap /handle %d 1 in order to accept "
         "or /handle %d 0 in order to decline request /\n",
         event->client_id, event->client_id, event->client_id);

  return 1;
};

unsigned int on_handle_challenge(unsigned int client_id, const void *data) {

  const struct ChallengeHandleEvent *event = data;

  if (event->accept) {
    printf("Your challenge request has been accepted, tap /grid in order to "
           "load the game\n");
  } else {
    printf("Your challenge request has been declined, try challenging other "
           "users !\n");
  }

  return 1;
};

unsigned int on_user_list(unsigned int client_id, const void *data) {

  const struct UserListRes *res = data;

  printf("Online users: %d\n", res->count);

  for (int i = 0; i < res->count; i++) {
    printf("User %s\n", res->users[i].name);
  }

  return 1;
};

unsigned int on_game_leave(unsigned int client_id, const void *data) {

  printf("The openent have left the game ! You can no longer play\n");

  return 1;
};

unsigned int on_error(unsigned int client_id, const void *data) {

  const struct ErrorEvent *event = data;

  printf("Error: %s\n", event->message);

  return 1;
};
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

unsigned int input_formatter_play(SOCKET sock, char *argv[],
                                  unsigned int argslen) {
  struct GamePlayReq req;

  req.input = atoi(argv[1]);

  char *cmd = inline_cmd(CMD_GAME_PLAY, &req, sizeof(struct GamePlayReq));

  write_to_socket(sock, cmd);

  free(cmd);

  return 1;
}

unsigned int input_formatter_challenge(SOCKET sock, char *argv[],
                                       unsigned int argslen) {
  struct ChallengeReq req;

  req.client_id = atoi(argv[1]);

  char *cmd = inline_cmd(CMD_CHALLENGE, &req, sizeof(struct ChallengeReq));

  write_to_socket(sock, cmd);

  free(cmd);

  return 1;
}

unsigned int input_formatter_handle(SOCKET sock, char *argv[],
                                    unsigned int argslen) {
  struct ChallengeHandleReq req;

  req.client_id = atoi(argv[1]);
  req.accept = atoi(argv[2]);

  char *cmd =
      inline_cmd(CMD_CHALLENGE_HANDLE, &req, sizeof(struct ChallengeHandleReq));

  write_to_socket(sock, cmd);

  free(cmd);

  return 1;
}

unsigned int input_formatter_grid(SOCKET sock, char *argv[],
                                  unsigned int argslen) {
  struct GameStateReq req;

  char *cmd = inline_cmd(CMD_GAME_STATE, &req, sizeof(struct GameStateReq));

  write_to_socket(sock, cmd);

  free(cmd);

  return 1;
}

unsigned int input_formatter_users(SOCKET sock, char *argv[],
                                   unsigned int argslen) {
  struct UserListReq req;

  req.page_number =
      argslen > 1
          ? atoi(argv[1])
          : 0; // par défaut page 0 // TODO: check whether the input is a number

  char *cmd = inline_cmd(CMD_USER_LIST_ALL, &req, sizeof(struct UserListReq));

  write_to_socket(sock, cmd);

  free(cmd);

  return 1;
}

unsigned int input_formatter_observe(SOCKET sock, char *argv[],
                                     unsigned int argslen) {
  struct UserObserveReq req;

  req.id = atoi(argv[1]);

  char *cmd = inline_cmd(CMD_GAME_OBSERVE, &req, sizeof(struct UserObserveReq));

  write_to_socket(sock, cmd);

  free(cmd);

  return 1;
}

unsigned int input_formatter_leave(SOCKET sock, char *argv[],
                                   unsigned int argslen) {

  struct GameLeaveReq req;

  char *cmd = inline_cmd(CMD_GAME_LEAVE, &req, sizeof(struct GameLeaveReq));

  write_to_socket(sock, cmd);

  free(cmd);

  return 1;
}

void init_client_mediator(struct ClientMediator *mediator) {

  register_client_cmd(mediator, "/chat", &input_formatter_chat);

  register_client_cmd(mediator, "/play", &input_formatter_play);

  register_client_cmd(mediator, "/challenge", &input_formatter_challenge);

  register_client_cmd(mediator, "/handle", &input_formatter_handle);

  register_client_cmd(mediator, "/grid", &input_formatter_grid);

  register_client_cmd(mediator, "/users", &input_formatter_users);

  register_client_cmd(mediator, "/observe", &input_formatter_observe);

  register_client_cmd(mediator, "/leave", &input_formatter_leave);
}

void init_mediator(struct Mediator *mediator) {
  // voir ces fonctions de manière inversée
  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);

  register_cmd(mediator, CMD_USER_LOGOUT, &on_user_logout);

  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write);

  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);

  register_cmd(mediator, CMD_CHALLENGE, &on_challenge);

  register_cmd(mediator, CMD_CHALLENGE_HANDLE, &on_handle_challenge);

  register_cmd(mediator, CMD_USER_LIST_ALL, &on_user_list);

  // pareil qu'avec le game state
  register_cmd(mediator, CMD_GAME_OBSERVE, &on_game_state);

  register_cmd(mediator, CMD_GAME_LEAVE, &on_game_leave);

  register_cmd(mediator, CMD_ERROR, &on_error);
}
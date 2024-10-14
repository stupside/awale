#include "mediator.h"

#include <stdlib.h>
#include <string.h>

#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/game.h"
#include "lib/socket/cmds/user.h"

unsigned int client_chat(SOCKET sock, char *argv[], unsigned int argslen) {
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

  send_cmd_to(sock, CMD_CHAT_WRITE, &req, sizeof(struct ChatWriteReq));

  return 1;
}

unsigned int client_play(SOCKET sock, char *argv[], unsigned int argslen) {
  struct GamePlayReq req;
  req.input = atoi(argv[1]);

  send_cmd_to(sock, CMD_GAME_PLAY, &req, sizeof(struct GamePlayReq));

  return 1;
}

unsigned int client_challenge(SOCKET sock, char *argv[], unsigned int argslen) {
  struct ChallengeReq req;
  req.client_id = atoi(argv[1]);

  send_cmd_to(sock, CMD_CHALLENGE, &req, sizeof(struct ChallengeReq));

  return 1;
}

unsigned int client_challenge_handle(SOCKET sock, char *argv[],
                                     unsigned int argslen) {
  struct ChallengeHandleReq req;
  req.accept = atoi(argv[2]);
  req.client_id = atoi(argv[1]);

  send_cmd_to(sock, CMD_CHALLENGE_HANDLE, &req,
              sizeof(struct ChallengeHandleReq));

  return 1;
}

unsigned int client_game_grid(SOCKET sock, char *argv[], unsigned int argslen) {
  struct GameStateReq req;

  send_cmd_to(sock, CMD_GAME_STATE, &req, sizeof(struct GameStateReq));

  return 1;
}

unsigned int client_users(SOCKET sock, char *argv[], unsigned int argslen) {
  struct UserListReq req = {
      .page = argslen > 1 ? atoi(argv[1]) : 0,
  };

  send_cmd_to(sock, CMD_USER_LIST_ALL, &req, sizeof(struct UserListReq));

  return 1;
}

unsigned int client_game_observe(SOCKET sock, char *argv[],
                                 unsigned int argslen) {
  struct UserObserveReq req = {
      .client_id = atoi(argv[1]),
  };

  send_cmd_to(sock, CMD_GAME_OBSERVE, &req, sizeof(struct UserObserveReq));

  return 1;
}

unsigned int client_game_leave(SOCKET sock, char *argv[],
                               unsigned int argslen) {

  struct GameLeaveReq req;

  send_cmd_to(sock, CMD_GAME_LEAVE, &req, sizeof(struct GameLeaveReq));

  return 1;
}

void init_client_mediator(struct ClientMediator *mediator) {

  register_client_cmd(mediator, "/chat", &client_chat);

  register_client_cmd(mediator, "/play", &client_play);

  register_client_cmd(mediator, "/challenge", &client_challenge);

  register_client_cmd(mediator, "/handle", &client_challenge_handle);

  register_client_cmd(mediator, "/grid", &client_game_grid);

  register_client_cmd(mediator, "/users", &client_users);

  register_client_cmd(mediator, "/observe", &client_game_observe);

  register_client_cmd(mediator, "/leave", &client_game_leave);
}
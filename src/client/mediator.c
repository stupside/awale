#include "mediator.h"

#include <stdlib.h>
#include <string.h>

#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/game.h"
#include "lib/socket/cmds/user.h"

unsigned int client_chat(SOCKET sock, char *argv[], unsigned int argslen) {

  struct ChatWriteReq req;

  char message[255];

  message[0] = '\0';

  for (unsigned int i = 1; i < argslen; i++) {

    strcat(message, argv[i]);

    if (i < argslen - 1) {
      strcat(message, " ");
    }
  }

  strncpy(req.message, message, sizeof(req.message) - 1);

  send_cmd_to(sock, CMD_CHAT_WRITE, &req, sizeof(struct ChatWriteReq));

  return 1;
}

unsigned int client_game_play(SOCKET sock, char *argv[], unsigned int argslen) {

  if (argslen < 2) {
    return 0;
  }

  struct GamePlayReq req;
  req.input = atoi(argv[1]);

  send_cmd_to(sock, CMD_GAME_PLAY, &req, sizeof(struct GamePlayReq));

  return 1;
}

unsigned int client_challenge(SOCKET sock, char *argv[], unsigned int argslen) {
  struct ChallengeReq req;

  if (argslen < 2) {
    return 0;
  }

  req.client_id = atoi(argv[1]);

  send_cmd_to(sock, CMD_CHALLENGE, &req, sizeof(struct ChallengeReq));

  return 1;
}

unsigned int client_challenge_accept(SOCKET sock, char *argv[],
                                     unsigned int argslen) {
  if (argslen < 2) {
    return 0;
  }

  struct ChallengeHandleReq req = {
      .accept = 1,
      .client_id = atoi(argv[1]),
  };

  send_cmd_to(sock, CMD_CHALLENGE_HANDLE, &req,
              sizeof(struct ChallengeHandleReq));

  return 1;
}

unsigned int client_challenge_reject(SOCKET sock, char *argv[],
                                     unsigned int argslen) {
  if (argslen < 2) {
    return 0;
  }

  struct ChallengeHandleReq req = {
      .accept = 0,
      .client_id = atoi(argv[1]),
  };

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
  if (argslen < 2) {
    return 0;
  }

  struct GameObserveReq req = {
      .observe = 1,
      .client_id = atoi(argv[1]),
  };

  send_cmd_to(sock, CMD_GAME_OBSERVE, &req, sizeof(struct GameObserveReq));

  return 1;
}

unsigned int client_game_unobserver(SOCKET sock, char *argv[],
                                    unsigned int argslen) {
  if (argslen < 2) {
    return 0;
  }

  struct GameObserveReq req = {
      .observe = 0,
      .client_id = atoi(argv[1]),
  };

  send_cmd_to(sock, CMD_GAME_OBSERVE, &req, sizeof(struct GameObserveReq));

  return 1;
}

unsigned int client_game_leave(SOCKET sock, char *argv[],
                               unsigned int argslen) {

  struct GameLeaveReq req;

  send_cmd_to(sock, CMD_GAME_LEAVE, &req, sizeof(struct GameLeaveReq));

  return 1;
}

unsigned int client_set_password(SOCKET sock, char *argv[],
                                 unsigned int argslen) {

  if (argslen < 2) {
    return 0;
  }

  struct UserSetInfoReq req = {0};

  strncpy(req.password, argv[1], sizeof(req.password) - 1);

  send_cmd_to(sock, CMD_USER_SET_INFO, &req, sizeof(struct UserSetInfoReq));

  return 1;
}

unsigned int client_set_username(SOCKET sock, char *argv[],
                                 unsigned int argslen) {

  if (argslen < 2) {
    return 0;
  }

  struct UserSetInfoReq req = {0};

  strncpy(req.name, argv[1], sizeof(req.name) - 1);

  send_cmd_to(sock, CMD_USER_SET_INFO, &req, sizeof(struct UserSetInfoReq));

  return 1;
}

unsigned int client_set_description(SOCKET sock, char *argv[],
                                    unsigned int argslen) {

  if (argslen < 2) {
    return 0;
  }

  struct UserSetInfoReq req = {0};

  unsigned int curs = 1;
  while (curs < argslen) {
    strncat(req.description, argv[curs],
            sizeof(req.description) - strlen(req.description) - 1);
    if (curs < argslen - 1) {
      strncat(req.description, " ",
              sizeof(req.description) - strlen(req.description) - 1);
    }
    curs++;
  }

  send_cmd_to(sock, CMD_USER_SET_INFO, &req, sizeof(struct UserSetInfoReq));

  return 1;
}

unsigned int client_user(SOCKET sock, char *argv[], unsigned int argslen) {
  struct UserGetInfoReq req;

  send_cmd_to(sock, CMD_USER_GET_INFO, &req, sizeof(struct UserGetInfoReq));

  return 1;
}

void init_client_mediator(struct ClientMediator *mediator) {

  register_client_cmd(mediator, "/chat", &client_chat);

  register_client_cmd(mediator, "/user", &client_user);
  register_client_cmd(mediator, "/users", &client_users);

  register_client_cmd(mediator, "/game-play", &client_game_play);
  register_client_cmd(mediator, "/game-grid", &client_game_grid);
  register_client_cmd(mediator, "/game-leave", &client_game_leave);
  register_client_cmd(mediator, "/game-observe", &client_game_observe);
  register_client_cmd(mediator, "/game-unobserve", &client_game_unobserver);

  register_client_cmd(mediator, "/challenge", &client_challenge);
  register_client_cmd(mediator, "/challenge-accept", &client_challenge_accept);
  register_client_cmd(mediator, "/challenge-reject", &client_challenge_reject);

  register_client_cmd(mediator, "/set-username", &client_set_username);
  register_client_cmd(mediator, "/set-password", &client_set_password);
  register_client_cmd(mediator, "/set-description", &client_set_description);
}
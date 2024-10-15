#include "mediator.h"

#include <stdlib.h>
#include <string.h>

#include "handlers/user.h"
#include "utils/string.h"

#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/game.h"
#include "lib/socket/cmds/user.h"

#include "lib/display/colors.h"

unsigned int client_chat(SOCKET sock, char *argv[], unsigned int argslen) {
  struct ChatWriteReq req;
  char message[255];

  if (is_number(argv[1])) {
    PRINT_COLOR(COLOR_YELLOW, "Sending message to client %d\n", atoi(argv[1]));
    req.client_id = atoi(argv[1]);
  } else {
    PRINT_COLOR(COLOR_YELLOW, "Sending message to all clients\n");
    req.client_id = -1;
  }

  message[0] = '\0';
  for (unsigned int i = 2; i < argslen; i++) {
    if (strlen(message) + strlen(argv[i]) + 1 >= sizeof(message)) {
      break;
    }
    strncat(message, argv[i], sizeof(message) - strlen(message) - 1);
    if (i < argslen - 1) {
      strncat(message, " ", sizeof(message) - strlen(message) - 1);
    }
  }

  strncpy(req.message, message, sizeof(req.message) - 1);
  req.message[sizeof(req.message) - 1] = '\0'; // Ensure null-termination

  if (send_cmd_to(sock, CMD_CHAT_WRITE, &req, sizeof(struct ChatWriteReq)) <
      0) {
    perror("Failed to send chat message");
    return 0;
  }
  return 1;
}

unsigned int client_game_play(SOCKET sock, char *argv[], unsigned int argslen) {
  if (argslen < 2)
    return 0;

  struct GamePlayReq req;
  req.input = atoi(argv[1]);

  send_cmd_to(sock, CMD_GAME_PLAY, &req, sizeof(struct GamePlayReq));
  return 1;
}

unsigned int client_challenge(SOCKET sock, char *argv[], unsigned int argslen) {
  struct ChallengeReq req;

  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "Usage: /challenge [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "Invalid client ID\n");
    return 0;
  }

  req.client_id = atoi(argv[1]);
  send_cmd_to(sock, CMD_CHALLENGE, &req, sizeof(struct ChallengeReq));
  return 1;
}

unsigned int client_challenge_accept(SOCKET sock, char *argv[],
                                     unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "Usage: /challenge-accept [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "Invalid client ID\n");
    return 0;
  }

  struct ChallengeHandleReq req = {.accept = 1, .client_id = atoi(argv[1])};
  send_cmd_to(sock, CMD_CHALLENGE_HANDLE, &req,
              sizeof(struct ChallengeHandleReq));
  return 1;
}

unsigned int client_challenge_reject(SOCKET sock, char *argv[],
                                     unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "Usage: /challenge-reject [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "Invalid client ID\n");
    return 0;
  }

  struct ChallengeHandleReq req = {.accept = 0, .client_id = atoi(argv[1])};
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
  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "Invalid page number\n");
    return 0;
  }

  struct UserListReq req = {.page = argslen > 1 ? atoi(argv[1]) : 0};
  send_cmd_to(sock, CMD_USER_LIST_ALL, &req, sizeof(struct UserListReq));
  return 1;
}

unsigned int client_game_observe(SOCKET sock, char *argv[],
                                 unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "Usage: /game-observe [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "Invalid client ID\n");
    return 0;
  }

  struct GameObserveReq req = {.observe = 1, .client_id = atoi(argv[1])};
  send_cmd_to(sock, CMD_GAME_OBSERVE, &req, sizeof(struct GameObserveReq));
  return 1;
}

unsigned int client_game_unobserve(SOCKET sock, char *argv[],
                                   unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "Usage: /game-unobserve [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "Invalid client ID\n");
    return 0;
  }

  struct GameObserveReq req = {.observe = 0, .client_id = atoi(argv[1])};
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
    PRINT_COLOR(COLOR_RED, "Usage: /set-password [password]\n");
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
    PRINT_COLOR(COLOR_RED, "Usage: /set-username [username]\n");
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
    PRINT_COLOR(COLOR_RED, "Usage: /set-description [description]\n");
    return 0;
  }

  struct UserSetInfoReq req = {0};
  for (unsigned int i = 1; i < argslen; i++) {
    strncat(req.description, argv[i],
            sizeof(req.description) - strlen(req.description) - 1);
    if (i < argslen - 1) {
      strncat(req.description, " ",
              sizeof(req.description) - strlen(req.description) - 1);
    }
  }

  send_cmd_to(sock, CMD_USER_SET_INFO, &req, sizeof(struct UserSetInfoReq));
  return 1;
}

unsigned int client_user(SOCKET sock, char *argv[], unsigned int argslen) {
  struct UserGetInfoReq req;

  if (argslen < 2) {
    PRINT_COLOR(COLOR_YELLOW, "Getting own user info\n");
    req.client_id = CLIENT_ID;
  } else {
    PRINT_COLOR(COLOR_YELLOW, "Getting user info for %d\n", atoi(argv[1]));
    req.client_id = atoi(argv[1]);
  }

  send_cmd_to(sock, CMD_USER_GET_INFO, &req, sizeof(struct UserGetInfoReq));
  return 1;
}

unsigned int client_help(SOCKET sock, char *argv[], unsigned int argslen) {
  PRINT_COLOR(COLOR_GREEN, "Available commands:\n");

  PRINT_COLOR(COLOR_YELLOW, "/help : Show this help\n");

  PRINT_COLOR(COLOR_YELLOW, "/chat [client_id] [message] : Send a message\n");

  PRINT_COLOR(COLOR_YELLOW, "/user [client_id] : Get user info\n");
  PRINT_COLOR(COLOR_YELLOW, "/users [page] : List users\n");

  PRINT_COLOR(COLOR_YELLOW, "/game-play [input] : Play a turn\n");
  PRINT_COLOR(COLOR_YELLOW, "/game-grid : Show game state\n");
  PRINT_COLOR(COLOR_YELLOW, "/game-leave : Leave the game\n");
  PRINT_COLOR(COLOR_YELLOW, "/game-observe [client_id] : Observe a player\n");
  PRINT_COLOR(COLOR_YELLOW, "/game-unobserve [client_id] : Stop observing\n");

  PRINT_COLOR(COLOR_YELLOW, "/challenge [client_id] : Challenge a player\n");
  PRINT_COLOR(COLOR_YELLOW,
              "/challenge-accept [client_id] : Accept challenge\n");
  PRINT_COLOR(COLOR_YELLOW,
              "/challenge-reject [client_id] : Reject challenge\n");

  PRINT_COLOR(COLOR_YELLOW, "/set-username [username] : Set username\n");
  PRINT_COLOR(COLOR_YELLOW, "/set-password [password] : Set password\n");
  PRINT_COLOR(COLOR_YELLOW,
              "/set-description [description] : Set description\n");

  return 1;
}

void init_client_mediator(struct ClientMediator *mediator) {
  register_client_cmd(mediator, "/help", &client_help);

  register_client_cmd(mediator, "/chat", &client_chat);
  register_client_cmd(mediator, "/user", &client_user);
  register_client_cmd(mediator, "/users", &client_users);

  register_client_cmd(mediator, "/game-play", &client_game_play);
  register_client_cmd(mediator, "/game-grid", &client_game_grid);
  register_client_cmd(mediator, "/game-leave", &client_game_leave);
  register_client_cmd(mediator, "/game-observe", &client_game_observe);
  register_client_cmd(mediator, "/game-unobserve", &client_game_unobserve);

  register_client_cmd(mediator, "/challenge", &client_challenge);
  register_client_cmd(mediator, "/challenge-accept", &client_challenge_accept);
  register_client_cmd(mediator, "/challenge-reject", &client_challenge_reject);

  register_client_cmd(mediator, "/set-username", &client_set_username);
  register_client_cmd(mediator, "/set-password", &client_set_password);
  register_client_cmd(mediator, "/set-description", &client_set_description);
}

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
  char message[255] = {0};

  if (is_number(argv[1])) {
    req.client_id = atoi(argv[1]);
    PRINT_COLOR(COLOR_YELLOW, "📨 Sending message to client %d...\n",
                req.client_id);
  } else {
    req.client_id = -1;
    PRINT_COLOR(COLOR_YELLOW, "📨 Broadcasting message to all clients...\n");
  }

  for (unsigned int i = 2; i < argslen; i++) {
    if (strlen(message) + strlen(argv[i]) + 1 >= sizeof(message)) {
      PRINT_COLOR(COLOR_RED, "⚠️ Message too long! Truncating...\n");
      break;
    }
    strncat(message, argv[i], sizeof(message) - strlen(message) - 1);
    if (i < argslen - 1)
      strncat(message, " ", sizeof(message) - strlen(message) - 1);
  }

  strncpy(req.message, message, sizeof(req.message) - 1);
  req.message[sizeof(req.message) - 1] = '\0';

  if (send_cmd_to(sock, CMD_CHAT_WRITE, &req, sizeof(req)) < 0) {
    perror("❌ Failed to send chat message");
    return 0;
  }
  PRINT_COLOR(COLOR_GREEN, "✅ Message sent successfully!\n");
  return 1;
}

unsigned int client_game_play(SOCKET sock, char *argv[], unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /game-play [position]\n");
    PRINT_COLOR(COLOR_YELLOW, "📝 Hint: Provide a valid pit number (0-5).\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "❌ Invalid input! Position must be a number.\n");
    return 0;
  }

  struct GamePlayReq req = {.input = atoi(argv[1])};
  send_cmd_to(sock, CMD_GAME_PLAY, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "✅ Move submitted successfully.\n");
  return 1;
}

unsigned int client_challenge(SOCKET sock, char *argv[], unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /challenge [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "❌ Invalid client ID. Must be a number.\n");
    return 0;
  }

  struct ChallengeReq req = {.client_id = atoi(argv[1])};
  send_cmd_to(sock, CMD_CHALLENGE, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "✅ Challenge sent to client %d.\n", req.client_id);
  return 1;
}

unsigned int client_challenge_accept(SOCKET sock, char *argv[],
                                     unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /challenge-accept [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "❌ Invalid client ID.\n");
    return 0;
  }

  struct ChallengeHandleReq req = {.accept = 1, .client_id = atoi(argv[1])};
  send_cmd_to(sock, CMD_CHALLENGE_HANDLE, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "✅ Challenge accepted.\n");
  return 1;
}

unsigned int client_challenge_reject(SOCKET sock, char *argv[],
                                     unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /challenge-reject [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "❌ Invalid client ID.\n");
    return 0;
  }

  struct ChallengeHandleReq req = {.accept = 0, .client_id = atoi(argv[1])};
  send_cmd_to(sock, CMD_CHALLENGE_HANDLE, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "✅ Challenge rejected.\n");
  return 1;
}

unsigned int client_game_grid(SOCKET sock, char *argv[], unsigned int argslen) {
  struct GameStateReq req;
  send_cmd_to(sock, CMD_GAME_STATE, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "✅ Game state requested.\n");
  return 1;
}

unsigned int client_users(SOCKET sock, char *argv[], unsigned int argslen) {
  unsigned int page = 0;

  if (argslen > 1 && is_number(argv[1])) {
    page = atoi(argv[1]);
    if (page < 0) {
      PRINT_COLOR(COLOR_RED, "❌ Invalid page number. Must be 0 or higher.\n");
      return 0;
    }
  }

  struct UserListReq req = {.page = page};
  send_cmd_to(sock, CMD_USER_LIST_ALL, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "🔍 Requesting user list (Page %d)...\n", page);
  return 1;
}

unsigned int client_game_observe(SOCKET sock, char *argv[],
                                 unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /game-observe [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "❌ Invalid client ID.\n");
    return 0;
  }

  struct GameObserveReq req = {.observe = 1, .client_id = atoi(argv[1])};
  send_cmd_to(sock, CMD_GAME_OBSERVE, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "👁️ Now observing client %d.\n", req.client_id);
  return 1;
}

unsigned int client_game_unobserve(SOCKET sock, char *argv[],
                                   unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /game-unobserve [client_id]\n");
    return 0;
  }

  if (!is_number(argv[1])) {
    PRINT_COLOR(COLOR_RED, "❌ Invalid client ID.\n");
    return 0;
  }

  struct GameObserveReq req = {.observe = 0, .client_id = atoi(argv[1])};
  send_cmd_to(sock, CMD_GAME_OBSERVE, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "👁️ Stopped observing client %d.\n", req.client_id);
  return 1;
}

unsigned int client_set_password(SOCKET sock, char *argv[],
                                 unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /set-password [password]\n");
    return 0;
  }

  struct UserSetInfoReq req = {0};
  strncpy(req.password, argv[1], sizeof(req.password) - 1);
  send_cmd_to(sock, CMD_USER_SET_INFO, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "✅ Password updated.\n");
  return 1;
}

unsigned int client_user(SOCKET sock, char *argv[], unsigned int argslen) {
  struct UserGetInfoReq req;

  if (argslen < 2) {
    PRINT_COLOR(COLOR_YELLOW, "🧑 Requesting your user info...\n");
    req.client_id = current_client_id();
  } else {
    req.client_id = atoi(argv[1]);
    PRINT_COLOR(COLOR_YELLOW, "🧑 Requesting user info for client %d...\n",
                req.client_id);
  }

  send_cmd_to(sock, CMD_USER_GET_INFO, &req, sizeof(req));
  return 1;
}

unsigned int client_game_leave(SOCKET sock, char *argv[],
                               unsigned int argslen) {
  struct GameLeaveReq req;
  send_cmd_to(sock, CMD_GAME_LEAVE, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "🚪 You have left the game.\n");
  return 1;
}

// ✏️ Set a new username
unsigned int client_set_username(SOCKET sock, char *argv[],
                                 unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /set-username [username]\n");
    return 0;
  }

  struct UserSetInfoReq req = {0};
  strncpy(req.name, argv[1], sizeof(req.name) - 1);
  send_cmd_to(sock, CMD_USER_SET_INFO, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "✅ Username updated to '%s'.\n", argv[1]);
  return 1;
}

unsigned int client_set_description(SOCKET sock, char *argv[],
                                    unsigned int argslen) {
  if (argslen < 2) {
    PRINT_COLOR(COLOR_RED, "⚠️ Usage: /set-description [description]\n");
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

  send_cmd_to(sock, CMD_USER_SET_INFO, &req, sizeof(req));
  PRINT_COLOR(COLOR_GREEN, "✅ Description updated.\n");
  return 1;
}

unsigned int client_help(SOCKET sock, char *argv[], unsigned int argslen) {
  PRINT_COLOR(COLOR_GREEN, "📜 **Available Commands:**\n");

  PRINT_COLOR(COLOR_YELLOW, "💬 **Chat Commands:**\n");
  PRINT_COLOR(
      COLOR_WHITE,
      "  • /chat [client_id] [message] : Send a message to a client (or all).\n"
      "  • /user [client_id] : Get info about yourself or another user.\n"
      "  • /users [page] : List users (optional page number).\n");

  PRINT_COLOR(COLOR_YELLOW, "🎮 **Game Commands:**\n");
  PRINT_COLOR(COLOR_WHITE,
              "  • /game-play [input] : Make a move in the game.\n"
              "  • /game-grid : Show the current game state.\n"
              "  • /game-leave : Exit the current game.\n"
              "  • /game-observe [client_id] : Watch another player's game.\n"
              "  • /game-unobserve [client_id] : Stop watching a game.\n");

  PRINT_COLOR(COLOR_YELLOW, "⚔️ **Challenge Commands:**\n");
  PRINT_COLOR(COLOR_WHITE,
              "  • /challenge [client_id] : Challenge a player.\n"
              "  • /challenge-accept [client_id] : Accept a challenge.\n"
              "  • /challenge-reject [client_id] : Reject a challenge.\n");

  PRINT_COLOR(COLOR_YELLOW, "🛠️ **User Management Commands:**\n");
  PRINT_COLOR(
      COLOR_WHITE,
      "  • /set-username [username] : Change your username.\n"
      "  • /set-password [password] : Change your password.\n"
      "  • /set-description [description] : Update your description.\n");

  PRINT_COLOR(COLOR_YELLOW, "❓ **General Commands:**\n");
  PRINT_COLOR(COLOR_WHITE, "  • /help : Show this help message.\n");

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

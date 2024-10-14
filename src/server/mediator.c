#include "mediator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/game.h"
#include "lib/socket/cmds/user.h"

#include "lib/awale/awale.h"

unsigned int on_user_login(unsigned int socket, const void *data) {

  struct Server *server = awale_server();

  const struct UserLoginReq *req = data;

  const SocketClient *existing = find_client_by_name(&server->pool, req->name);

  unsigned int client_id = 0;

  if (existing) {

    if (existing->online) {
      perror("Client already online");
      return 0;
    }

    if (strcmp(existing->password, req->password) != 0) {
      perror("Invalid password");
      return 0;
    }

    const int ok =
        unarchive_client(&server->pool, socket, req->name, &client_id);

    if (!ok) {
      perror("Failed to unarchive client");
      return 0;
    }
  } else {
    const int ok =
        add_client(&server->pool, req->name, req->password, socket, &client_id);

    if (!ok) {
      perror("Failed to add client");
      return 0;
    }
  }

  const struct UserLoginEvent event = {.id = client_id};

  char *cmd = inline_cmd(CMD_USER_LOGIN, &event, sizeof(struct UserLoginEvent));

  const SocketClient *sender =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!sender) {
    return 0;
  }

  write_to_sockets(&awale_server()->pool, sender, cmd);

  free(cmd);

  return 1;
};

unsigned int on_user_logout(unsigned int client_id, const void *data) {

  const SocketClient *sender =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!sender) {
    return 0;
  }

  {
    const struct UserLogoutEvent event = {.id = sender->id};

    char *cmd =
        inline_cmd(CMD_USER_LOGOUT, &event, sizeof(struct UserLogoutEvent));

    /**
     * Tells all clients that a user has logged out
     */
    write_to_sockets(&awale_server()->pool, sender, cmd);

    free(cmd);
  }

  return 1;
};

unsigned int on_chat_write(unsigned int client_id, const void *data) {

  const struct ChatWriteReq *req = data;

  const SocketClient *sender =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!sender) {
    return 0;
  }

  {
    struct ChatWriteEvent event = {.client_id = sender->id};
    strcpy(event.message, req->message);

    char *cmd =
        inline_cmd(CMD_CHAT_WRITE, &event, sizeof(struct ChatWriteEvent));

    write_to_sockets(&awale_server()->pool, sender, cmd);

    free(cmd);
  }

  return 1;
};

unsigned int on_challenge(unsigned int client_id, const void *data) {

  SocketClient *sender = find_client_by_id(&awale_server()->pool, client_id);

  if (!sender) {
    return 0;
  }

  const struct ChallengeReq *req = data;

  SocketClient *client =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!client) {
    return 0;
  }

  return challenge(awale_server(), sender, client);
};

unsigned int on_challenge_handle(unsigned int client_id, const void *data) {

  const struct ChallengeHandleReq *req = data;

  const SocketClient *challenger =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!challenger) {
    return 0;
  }

  const SocketClient *challenged =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!challenged) {
    return 0;
  }

  int ok =
      handle_challenge(awale_server(), challenger, challenged, req->accept);

  if (!ok) {
    return 0;
  }

  return 1;
};

unsigned int on_game_state(unsigned int client_id, const void *data) {

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  if (client == NULL) {
    return 0;
  }

  const struct Lobby *lobby = find_running_lobby(awale_server(), client);

  if (!lobby) {
    return 0;
  }

  struct GameStateRes res = {
      .status = status(&lobby->awale),
  };

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      res.grid[i][j] = lobby->awale.grid[i][j];
    }
  }

  char *cmd = inline_cmd(CMD_GAME_STATE, &res, sizeof(struct GameStateRes));

  write_to_socket(client->socket, cmd);

  free(cmd);

  return 1;
};

void init_mediator(struct Mediator *mediator) {
  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write);

  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);
  register_cmd(mediator, CMD_USER_LOGOUT, &on_user_logout);

  register_cmd(mediator, CMD_CHALLENGE, &on_challenge);
  register_cmd(mediator, CMD_CHALLENGE_HANDLE, &on_challenge_handle);

  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);
}
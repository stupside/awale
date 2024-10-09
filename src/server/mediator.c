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

unsigned int on_user_login(unsigned int client_id, const void *data) {

  {
    const struct UserLoginEvent event = {.id = client_id};

    char *cmd =
        inline_cmd(CMD_USER_LOGIN, &event, sizeof(struct UserLoginEvent));

    const SocketClient *sender = find_client(&awale_server()->pool, client_id);

    if (sender == NULL) {
      return 0;
    }

    /**
     * Tells all clients that a new user has logged in
     */
    write_to_sockets(&awale_server()->pool, sender, cmd);

    free(cmd);
  }

  return 1;
};

unsigned int on_user_logout(unsigned int client_id, const void *data) {

  const SocketClient *sender = find_client(&awale_server()->pool, client_id);

  if (sender == NULL) {
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

  const SocketClient *sender = find_client(&awale_server()->pool, client_id);

  if (sender == NULL) {
    return 0;
  }

  {
    struct ChatWriteEvent event = {.client_id = sender->id};
    strcpy(event.message, req->message);

    char *cmd =
        inline_cmd(CMD_CHAT_WRITE, &event, sizeof(struct ChatWriteEvent));

    /**
     * Broadcasts the message to all clients
     */
    write_to_sockets(&awale_server()->pool, sender, cmd);

    free(cmd);
  }

  return 1;
};

unsigned int on_challenge(unsigned int client_id, const void *data) {

  SocketClient *sender = find_client(&awale_server()->pool, client_id);

  if (sender == NULL) {
    return 0;
  }

  const struct ChallengeReq *req = data;

  SocketClient *client = find_client(&awale_server()->pool, req->client_id);

  int ok = challenge(awale_server(), sender, client);

  if (!ok) {
    return 0;
  }

  return 1;
};

unsigned int on_challenge_handle(unsigned int client_id, const void *data) {

  const SocketClient *client = find_client(&awale_server()->pool, client_id);

  if (client == NULL) {
    return 0;
  }

  const struct ChallengeHandleReq *req = data;

  int ok = handle_challenge(awale_server(), client, req->accept);

  if (!ok) {
    return 0;
  }

  return 1;
};

unsigned int on_game_state(unsigned int client_id, const void *data) {

  const SocketClient *client = find_client(&awale_server()->pool, client_id);

  if (client == NULL) {
    return 0;
  }

  const struct Lobby *lobby = find_lobby(awale_server(), client);

  if (lobby == NULL) {
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

  write_to_socket(client->sock, cmd);

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
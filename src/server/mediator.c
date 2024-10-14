#include "mediator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/error.h"

#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/game.h"
#include "lib/socket/cmds/user.h"

unsigned int on_user_login(unsigned int socket, const void *data) {

  struct Server *server = awale_server();

  const struct UserLoginReq *req = data;

  const SocketClient *existing = find_client_by_name(&server->pool, req->name);

  unsigned int client_id;

  if (existing) {

    if (existing->online) {
      const struct ErrorEvent event = {.message = "Client already online"};

      send_cmd_to(socket, CMD_ERROR, &event, sizeof(struct ErrorEvent));

      perror("Client already online");
      return 0;
    }

    if (strcmp(existing->password, req->password) != 0) {

      const struct ErrorEvent event = {.message = "Invalid password"};

      send_cmd_to(socket, CMD_ERROR, &event, sizeof(struct ErrorEvent));

      perror("Invalid password");
      return 0;
    }

    const int ok =
        unarchive_client(&server->pool, socket, req->name, &client_id);

    if (!ok) {

      const struct ErrorEvent event = {.message = "Failed to unarchive client"};

      send_cmd_to(socket, CMD_ERROR, &event, sizeof(struct ErrorEvent));

      perror("Failed to unarchive client");
      return 0;
    }
  } else {

    const int ok =
        add_client(&server->pool, req->name, req->password, socket, &client_id);

    if (!ok) {

      const struct ErrorEvent event = {.message = "Failed to add client"};

      send_cmd_to(socket, CMD_ERROR, &event, sizeof(struct ErrorEvent));

      perror("Failed to add client");
      return 0;
    }
  }

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!client) {
    return 0;
  }

  const struct UserLoginEvent event = {.id = client_id};

  send_cmd_to_all(&awale_server()->pool, client, CMD_USER_LOGIN, &event,
                  sizeof(struct UserLoginEvent));

  return 1;
};

unsigned int on_user_logout(unsigned int client_id, const void *data) {

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!client) {
    return 0;
  }

  {
    const struct UserLogoutEvent event = {.client_id = client->id};

    char *cmd =
        inline_cmd(CMD_USER_LOGOUT, &event, sizeof(struct UserLogoutEvent));

    send_cmd_to_all(&awale_server()->pool, client, CMD_USER_LOGOUT, &event,
                    sizeof(struct UserLogoutEvent));

    free(cmd);
  }

  return 1;
};

unsigned int on_chat_write(unsigned int client_id, const void *data) {

  const struct ChatWriteReq *req = data;

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!client) {
    return 0;
  }

  {
    struct ChatWriteEvent event = {.client_id = client->id};
    strcpy(event.message, req->message);

    send_cmd_to_all(&awale_server()->pool, client, CMD_CHAT_WRITE, &event,
                    sizeof(struct ChatWriteEvent));
  }

  return 1;
};

unsigned int on_challenge(unsigned int client_id, const void *data) {

  SocketClient *challenger =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!challenger) {
    return 0;
  }

  const struct ChallengeReq *req = data;

  SocketClient *challenged =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!challenged) {

    const struct ErrorEvent event = {.message = "Client not found"};

    send_cmd_to(challenger->socket, CMD_ERROR, &event,
                sizeof(struct ErrorEvent));

    return 0;
  }

  return challenge(awale_server(), challenger, challenged);
};

unsigned int on_challenge_handle(unsigned int client_id, const void *data) {

  const struct ChallengeHandleReq *req = data;

  const SocketClient *challenger =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!challenger) {

    const struct ErrorEvent event = {.message = "Challenger not found"};

    send_cmd_to(challenger->socket, CMD_ERROR, &event,
                sizeof(struct ErrorEvent));

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

  send_cmd_to(client->socket, CMD_GAME_STATE, &res,
              sizeof(struct GameStateRes));

  return 1;
};

unsigned int on_game_play(unsigned int client_id, const void *data) {

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  if (client == NULL) {
    return 0;
  }

  const struct GamePlayReq *req = data;

  struct Lobby *lobby = find_running_lobby(awale_server(), client);

  if (lobby == NULL) {
    return 0;
  }

  enum CoupValidity ok = play(&lobby->awale, client_id, req->input);

  const struct GamePlayRes event = {.validity = ok};

  send_cmd_to(client->socket, CMD_GAME_PLAY, &event,
              sizeof(struct GamePlayRes));

  if (ok != VALID) {
    return 0;
  }

  struct GameStateEvent res = {
      .status = status(&lobby->awale),
  };

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      res.grid[i][j] = lobby->awale.grid[i][j];
    }
  }

  send_cmd_to(lobby->client[PLAYER2]->socket, CMD_GAME_STATE_EVENT, &res,
              sizeof(struct GameStateEvent));

  send_cmd_to(lobby->client[PLAYER1]->socket, CMD_GAME_STATE_EVENT, &res,
              sizeof(struct GameStateEvent));

  return 1;
};

unsigned int on_user_list_all(unsigned int client_id, const void *data) {

  const struct UserListReq *req = data;

  struct UserListRes res;

  const unsigned int min = req->page * PAGE_MAX_CLIENTS;
  const unsigned int max =
      (min + PAGE_MAX_CLIENTS) % awale_server()->pool.count;

  for (unsigned int i = min; i < max; i++) {

    const SocketClient *client = &awale_server()->pool.clients[i];

    if (client->id == client_id) {
      continue;
    }

    if (client->online) {

      struct UserRes user = {
          .client_id = client->id,
          .description = {0},
      };

      strncpy(user.name, client->name, USER_NAME_LEN);

      res.users[res.count++] = user;
    }
  }

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  send_cmd_to(client->socket, CMD_USER_LIST_ALL, &res,
              sizeof(struct UserListRes));

  return 1;
};

void init_mediator(struct Mediator *mediator) {
  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write);

  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);
  register_cmd(mediator, CMD_USER_LOGOUT, &on_user_logout);

  register_cmd(mediator, CMD_CHALLENGE, &on_challenge);
  register_cmd(mediator, CMD_CHALLENGE_HANDLE, &on_challenge_handle);

  register_cmd(mediator, CMD_GAME_PLAY, &on_game_play);
  register_cmd(mediator, CMD_GAME_STATE, &on_game_state);

  register_cmd(mediator, CMD_USER_LIST_ALL, &on_user_list_all);
}
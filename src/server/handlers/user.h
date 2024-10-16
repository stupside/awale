#ifndef USER_HANDLERS_H
#define USER_HANDLERS_H

#include "lib/cmds/mediator.h"

#include <stdio.h>
#include <string.h>

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/error.h"

#include "lib/socket/cmds/user.h"

unsigned int on_user_login(unsigned int socket, const void *data) {

  struct Server *server = awale_server();

  const struct UserLoginReq *req = data;

  const SocketClient *existing = find_client_by_name(&server->pool, req->name);

  unsigned int client_id;

  if (existing) {

    if (existing->online) {

      perror("Client already online");
      return 0;
    }

    if (strcmp(existing->password, req->password) != 0) {

      perror("Invalid password");
      return 0;
    }

    const int unarchived =
        unarchive_client(&server->pool, socket, req->name, &client_id);

    if (!unarchived) {
      perror("Failed to unarchive client");
      return 0;
    }
  } else {

    const int added =
        add_client(&server->pool, req->name, req->password, socket, &client_id);

    if (!added) {
      perror("Failed to add client");
      return 0;
    }
  }

  const struct UserLoginRes res = {.client_id = client_id};

  send_cmd_to(socket, CMD_USER_LOGIN, &res, sizeof(struct UserLoginRes));

  const struct UserLoginEvent event = {.client_id = client_id};

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  send_cmd_to_pool(&awale_server()->pool, client, CMD_USER_LOGIN_EVENT, &event,
                   sizeof(struct UserLoginEvent));

  return 1;
};

unsigned int on_user_logout(unsigned int client_id, const void *data) {

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  {
    const struct UserLogoutEvent event = {.client_id = client->id};

    send_cmd_to_pool(&awale_server()->pool, client, CMD_USER_LOGOUT_EVENT,
                     &event, sizeof(struct UserLogoutEvent));
  }

  return 1;
};

unsigned int on_user_list_all(unsigned int client_id, const void *data) {

  const struct SocketClient *clients_online[awale_server()->pool.count];

  int clients_online_c = 0;

  for (unsigned int i = 0; i < awale_server()->pool.count; ++i) {

    const SocketClient *client = &awale_server()->pool.clients[i];

    if (client->id == client_id) {
      continue;
    }

    if (client->online) {
      clients_online[clients_online_c++] = client;
    }
  }

  const struct UserListReq *req = data;

  const unsigned int min = req->page * PAGE_MAX_CLIENTS;

  unsigned int max = clients_online_c / PAGE_MAX_CLIENTS == req->page
                         ? min + clients_online_c % PAGE_MAX_CLIENTS - 1
                         : PAGE_MAX_CLIENTS * (req->page + 1) - 1;

  struct UserListRes res = {
      .count = 0,
  };

  for (unsigned int i = min; i <= max; ++i) {

    const SocketClient *client = clients_online[i];

    struct UserRes user = {
        .client_id = client->id,
        .description = {0},
    };

    strncpy(user.name, client->name, USER_NAME_LEN);

    res.users[res.count++] = user;
  }

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  send_cmd_to(client->socket, CMD_USER_LIST_ALL, &res,
              sizeof(struct UserListRes));

  return 1;
};

unsigned int on_user_set_info(unsigned int client_id, const void *data) {

  const struct UserSetInfoReq *req = data;

  SocketClient *client = find_client_by_id(&awale_server()->pool, client_id);

  if (req->name[0] != '\0') {

    if (find_client_by_name(&awale_server()->pool, req->name)) {
      return 0;
    } else {
      strncpy(client->name, req->name, USER_NAME_LEN);
    }
  }

  if (req->password[0] != '\0') {
    strncpy(client->password, req->password, USER_PASSWORD_LEN);
  }

  if (req->description[0] != '\0') {
    strncpy(client->description, req->description, USER_DESC_LEN);
  }

  const struct UserSetInfoRes res = {};

  send_cmd_to(client->socket, CMD_USER_SET_INFO, &res,
              sizeof(struct UserSetInfoRes));

  return 1;
};

unsigned int on_user_get_info(unsigned int client_id, const void *data) {

  const struct UserGetInfoReq *req = data;

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  const SocketClient *client_to_get =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!client_to_get) {

    send_error_to_client(client, ERROR_CLIENT_NOT_FOUND);

    return 0;
  }

  struct UserGetInfoRes res = {
      .user.client_id = client_to_get->id,
  };

  strncpy(res.user.name, client_to_get->name, USER_NAME_LEN);
  strncpy(res.user.description, client_to_get->description, USER_DESC_LEN);

  send_cmd_to(client->socket, CMD_USER_GET_INFO, &res,
              sizeof(struct UserGetInfoRes));

  return 1;
};

void add_user_cmds(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);
  register_cmd(mediator, CMD_USER_LOGOUT, &on_user_logout);
  register_cmd(mediator, CMD_USER_LIST_ALL, &on_user_list_all);
  register_cmd(mediator, CMD_USER_SET_INFO, &on_user_set_info);
  register_cmd(mediator, CMD_USER_GET_INFO, &on_user_get_info);
}

#endif
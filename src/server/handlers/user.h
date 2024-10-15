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
      const struct ErrorEvent event = {.message = "Client already online"};

      send_cmd_to(socket, CMD_ERROR_EVENT, &event, sizeof(struct ErrorEvent));

      perror("Client already online");
      return 0;
    }

    if (strcmp(existing->password, req->password) != 0) {

      const struct ErrorEvent event = {.message = "Invalid password"};

      send_cmd_to(socket, CMD_ERROR_EVENT, &event, sizeof(struct ErrorEvent));

      perror("Invalid password");
      return 0;
    }

    const int ok =
        unarchive_client(&server->pool, socket, req->name, &client_id);

    if (!ok) {

      const struct ErrorEvent event = {.message = "Failed to unarchive client"};

      send_cmd_to(socket, CMD_ERROR_EVENT, &event, sizeof(struct ErrorEvent));

      perror("Failed to unarchive client");
      return 0;
    }
  } else {

    const int ok =
        add_client(&server->pool, req->name, req->password, socket, &client_id);

    if (!ok) {

      const struct ErrorEvent event = {.message = "Failed to add client"};

      send_cmd_to(socket, CMD_ERROR_EVENT, &event, sizeof(struct ErrorEvent));

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

  send_cmd_to_all(&awale_server()->pool, client, CMD_USER_LOGIN_EVENT, &event,
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

    send_cmd_to_all(&awale_server()->pool, client, CMD_USER_LOGOUT_EVENT,
                    &event, sizeof(struct UserLogoutEvent));
  }

  return 1;
};

unsigned int on_user_list_all(unsigned int client_id, const void *data) {

  // on filtre
  const struct SocketClient *socketClients[awale_server()->pool.count];
  int countParseableUsers = 0;
  for (unsigned int i = 0; i < awale_server()->pool.count; i++) {
    const SocketClient *client = &awale_server()->pool.clients[i];
    if (client->id == client_id) {
      continue;
    }
    if (client->online) {
      socketClients[countParseableUsers++] = client;
    }
  }

  const struct UserListReq *req = data;

  struct UserListRes res = {
      .count = 0,
  };

  const unsigned int min = req->page * PAGE_MAX_CLIENTS;

  unsigned int max = PAGE_MAX_CLIENTS * (req->page + 1) - 1;

  if (countParseableUsers / PAGE_MAX_CLIENTS == req->page) {
    max = min + countParseableUsers % PAGE_MAX_CLIENTS - 1;
  }

  for (unsigned int i = min; i <= max; i++) {

    const SocketClient *client = socketClients[i];

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

void add_user_cmds(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);
  register_cmd(mediator, CMD_USER_LOGOUT, &on_user_logout);
  register_cmd(mediator, CMD_USER_LIST_ALL, &on_user_list_all);
}

#endif
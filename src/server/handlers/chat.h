#ifndef CHAT_HANDLERS_H
#define CHAT_HANDLERS_H

#include "lib/cmds/mediator.h"

#include <string.h>

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/chat.h"
#include "lib/socket/cmds/error.h"

unsigned int on_chat_write_event(unsigned int client_id, const void *data) {

  const struct ChatWriteReq *req = data;

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  unsigned int receiver_count = 0;

  struct ChatWriteEvent event = {.client_id = client->id};
  strcpy(event.message, req->message);
  if (req->client_id == -1) {

    send_cmd_to_pool(&awale_server()->pool, client, CMD_CHAT_WRITE_EVENT,
                     &event, sizeof(struct ChatWriteEvent));

    receiver_count = awale_server()->pool.count - 1;

  } else {
    const SocketClient *receiver =
        find_client_by_id(&awale_server()->pool, req->client_id);

    if (receiver) {
      send_cmd_to_client(receiver, CMD_CHAT_WRITE_EVENT, &event,
                         sizeof(struct ChatWriteEvent));
    } else {

      send_error_to_client(client, ERROR_CLIENT_NOT_FOUND);

      return 0;
    }

    receiver_count = 1;
  }

  const struct ChatWriteRes res = {
      .count = receiver_count,
  };

  send_cmd_to_client(client, CMD_CHAT_WRITE, &res, sizeof(struct ChatWriteRes));

  return 1;
};

void add_chat_cmds(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write_event);
}

#endif
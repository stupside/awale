#ifndef CHAT_HANDLERS_H
#define CHAT_HANDLERS_H

#include "lib/cmds/mediator.h"

#include <string.h>

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/chat.h"

unsigned int on_chat_write(unsigned int client_id, const void *data) {

  const struct ChatWriteReq *req = data;

  const SocketClient *client =
      find_client_by_id(&awale_server()->pool, client_id);

  if (!client) {
    return 0;
  }

  struct ChatWriteEvent event = {.client_id = client->id};
  strcpy(event.message, req->message);

  if (req->client_id == -1) {

    send_cmd_to_pool(&awale_server()->pool, client, CMD_CHAT_WRITE_EVENT,
                     &event, sizeof(struct ChatWriteEvent));
    return 1;
  }

  const SocketClient *receiver =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!receiver) {
    return 0;
  }

  send_cmd_to_client(receiver, CMD_CHAT_WRITE_EVENT, &event,
                     sizeof(struct ChatWriteEvent));

  return 1;
};

void add_chat_cmds(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write);
}

#endif
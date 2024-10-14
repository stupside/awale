#ifndef CHALLENGE_HANDLERS_H
#define CHALLENGE_HANDLERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/cmds/mediator.h"

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/error.h"

#include "lib/socket/cmds/challenge.h"

unsigned int on_challenge_event(unsigned int client_id, const void *data) {

  SocketClient *challenger =
      find_client_by_id(&awale_server()->pool, client_id);

  const struct ChallengeReq *req = data;

  SocketClient *challenged =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!challenged) {

    const struct ErrorEvent event = {.message = "Client not found"};

    send_cmd_to(challenger->socket, CMD_ERROR_EVENT, &event,
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

    send_cmd_to(challenger->socket, CMD_ERROR_EVENT, &event,
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

void add_challenge_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_CHALLENGE, &on_challenge_event);
  register_cmd(mediator, CMD_CHALLENGE_HANDLE, &on_challenge_handle);
}

#endif
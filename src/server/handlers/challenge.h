#ifndef CHALLENGE_HANDLERS_H
#define CHALLENGE_HANDLERS_H

#include "lib/cmds/mediator.h"

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/error.h"

#include "lib/socket/cmds/challenge.h"

unsigned int on_challenge(unsigned int client_id, const void *data) {

  SocketClient *challenger =
      find_client_by_id(&awale_server()->pool, client_id);

  const struct ChallengeReq *req = data;

  SocketClient *challenged =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!challenged) {

    send_error_to_client(challenger, ERROR_CLIENT_NOT_FOUND);

    return 0;
  }

  unsigned int ok = challenge(awale_server(), challenger, challenged);

  if (ok) {
    return 1;
  }

  send_error_to_client(challenger, ERROR_FAILED_TO_CHALLENGE);

  return 0;
};

unsigned int on_challenge_handle(unsigned int client_id, const void *data) {

  const struct ChallengeHandleReq *req = data;

  const SocketClient *challenger =
      find_client_by_id(&awale_server()->pool, req->client_id);

  if (!challenger) {

    send_error_to_client(challenger, ERROR_FAILED_TO_RETRIEVE_CHALLENGER);

    return 0;
  }

  const SocketClient *challenged =
      find_client_by_id(&awale_server()->pool, client_id);

  int ok =
      handle_challenge(awale_server(), challenger, challenged, req->accept);

  if (ok) {
    return 1;
  }

  send_error_to_client(challenger, ERROR_FAILED_TO_HANDLE_CHALLENGE);

  return 0;
};

void add_challenge_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_CHALLENGE, &on_challenge, PERSIST);
  register_cmd(mediator, CMD_CHALLENGE_HANDLE, &on_challenge_handle, PERSIST);
}

#endif
#include "mediator.h"

#include <stdio.h>
#include <stdlib.h>

#include "lib/server/server.h"
#include "lib/socket/socket.h"

#include "lib/socket/cmds/challenge.h"
#include "lib/socket/cmds/chat.h"
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

  struct Lobby *lobby = find_lobby(awale_server(), sender);

  if (lobby) {
    return 0;
  }

  const struct ChallengeReq *req = data;

  SocketClient *client = find_client(&awale_server()->pool, req->client_id);

  int ok = challenge(awale_server(), sender, client);

  if (!ok) {
    return 0;
  }

  /**
   * Sends a challenge request to the other client
   */
  {
    const struct ChallengeEvent event = {.client_id = sender->id};

    char *cmd =
        inline_cmd(CMD_CHALLENGE, &event, sizeof(struct ChallengeEvent));

    write_to_socket(client->sock, cmd);

    free(cmd);
  }

  return 1;
};

unsigned int on_challenge_accept(unsigned int client_id, const void *data) {

  const SocketClient *from = find_client(&awale_server()->pool, client_id);

  if (from == NULL) {
    return 0;
  }

  struct Lobby *lobby = find_lobby(awale_server(), from);

  if (lobby == NULL) {
    return 0;
  }

  if (lobby->state == LOBBY_STATE_PLAYING ||
      lobby->state == LOBBY_STATE_FINISHED) {
    return 0;
  }

  lobby->state = LOBBY_STATE_PLAYING;

  /**
   * Sends a challenge accept request to the other client
   */
  {
    const struct ChallengeAcceptEvent event;

    char *cmd = inline_cmd(CMD_CHALLENGE_ACCEPT, &event,
                           sizeof(struct ChallengeAcceptEvent));

    write_to_socket(lobby->client[PLAYER1]->sock, cmd);

    free(cmd);
  }

  return 1;
};

unsigned int on_challenge_reject(unsigned int client_id, const void *data) {

  const SocketClient *from = find_client(&awale_server()->pool, client_id);

  if (from == NULL) {
    return 0;
  }

  struct Lobby *lobby = find_lobby(awale_server(), from);

  if (lobby == NULL) {
    return 0;
  }

  if (lobby->state == LOBBY_STATE_PLAYING ||
      lobby->state == LOBBY_STATE_FINISHED) {
    return 0;
  }

  /**
   * Sends a challenge reject request to the other client
   */
  {
    const struct ChallengeDeclineEvent event;

    char *cmd = inline_cmd(CMD_CHALLENGE_REJECT, &event,
                           sizeof(struct ChallengeDeclineEvent));

    write_to_socket(lobby->client[PLAYER1]->sock, cmd);

    free(cmd);
  }

  return 1;
};

void init_mediator(struct Mediator *mediator) {
  register_cmd(mediator, CMD_CHAT_WRITE, &on_chat_write);

  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login);
  register_cmd(mediator, CMD_USER_LOGOUT, &on_user_logout);

  register_cmd(mediator, CMD_CHALLENGE, &on_challenge);
  register_cmd(mediator, CMD_CHALLENGE_ACCEPT, &on_challenge_accept);
  register_cmd(mediator, CMD_CHALLENGE_REJECT, &on_challenge_reject);
}
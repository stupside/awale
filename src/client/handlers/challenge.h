#ifndef CHALLENGE_HANDLERS_H
#define CHALLENGE_HANDLERS_H

#include <stdio.h>

#include "lib/cmds/mediator.h"
#include "lib/socket/cmd.h"

#include "lib/socket/cmds/challenge.h"

unsigned int on_challenge(unsigned int client_id, const void *data) {

  const struct ChallengeEvent *event = data;

  printf("You have been challenged by %d, tap /handle %d 1 in order to accept "
         "or /handle %d 0 in order to decline request /\n",
         event->client_id, event->client_id, event->client_id);

  return 1;
};

unsigned int on_handle_challenge_event(unsigned int client_id,
                                       const void *data) {

  const struct ChallengeHandleEvent *event = data;

  if (event->accept) {
    printf("Your challenge request has been accepted, tap /grid in order to "
           "load the game\n");
  } else {
    printf("Your challenge request has been declined, try challenging other "
           "users !\n");
  }

  return 1;
};

void add_challenge_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_CHALLENGE_EVENT, &on_challenge);
  register_cmd(mediator, CMD_CHALLENGE_HANDLE_EVENT,
               &on_handle_challenge_event);
}

#endif
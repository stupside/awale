#include "challenge.h"

#include "lib/display/colors.h"
#include "lib/socket/cmds/challenge.h"

#include "lib/socket/cmd.h"

unsigned int on_challenge(unsigned int client_id, const void *data) {
  const struct ChallengeEvent *event = data;

  PRINT_COLOR(COLOR_BLUE,
              "You have been challenged by %d, tap /handle %d 1 to accept or "
              "/handle %d 0 to decline.\n",
              event->client_id, event->client_id, event->client_id);

  return 1;
}

unsigned int on_handle_challenge_event(unsigned int client_id,
                                       const void *data) {
  const struct ChallengeHandleEvent *event = data;

  if (event->accept) {
    PRINT_COLOR(COLOR_GREEN, "Your challenge request has been accepted, tap "
                             "/grid to load the game.\n");
  } else {
    PRINT_COLOR(COLOR_RED, "Your challenge request has been declined.\n");
  }

  return 1;
}

void add_challenge_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_CHALLENGE_EVENT, &on_challenge);
  register_cmd(mediator, CMD_CHALLENGE_HANDLE_EVENT,
               &on_handle_challenge_event);
}
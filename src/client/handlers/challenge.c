#include "challenge.h"

#include "lib/display/colors.h"
#include "lib/socket/cmd.h"
#include "lib/socket/cmds/challenge.h"

unsigned int on_challenge(unsigned int client_id, const void *data) {
  const struct ChallengeEvent *event = data;

  PRINT_COLOR(COLOR_CYAN, // Changed to cyan for system notifications
              "📢 Player %d has challenged you!\n"
              "➡️ Use /challenge-accept %d to accept the challenge or\n"
              "❌ Use /challenge-reject %d to decline.\n",
              event->client_id, event->client_id, event->client_id);

  return 1;
}

unsigned int on_handle_challenge_event(unsigned int client_id,
                                       const void *data) {
  const struct ChallengeHandleEvent *event = data;

  if (event->accept) {
    PRINT_COLOR(COLOR_GREEN, "✅ Your challenge was accepted! Use /game-grid to "
                             "view the game board.\n");
  } else {
    PRINT_COLOR(COLOR_YELLOW, "⚠️ Your challenge was declined by the player.\n");
  }

  return 1;
}

void add_challenge_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_CHALLENGE_EVENT, &on_challenge);
  register_cmd(mediator, CMD_CHALLENGE_HANDLE_EVENT,
               &on_handle_challenge_event);
}

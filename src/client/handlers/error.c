#include "error.h"

#include "lib/display/colors.h"
#include "lib/socket/cmd.h"
#include "lib/socket/cmds/error.h"

unsigned int on_error_event(unsigned int client_id, const void *data) {
  const struct ErrorEvent *event = data;

  switch (event->code) {
  case ERROR_CLIENT_NOT_FOUND:
    PRINT_COLOR(COLOR_RED, "❌ Error: No available client was found.\n");
    break;
  case ERROR_FAILED_TO_CHALLENGE:
    PRINT_COLOR(COLOR_RED, "❌ Error: Could not send the challenge.\n");
    break;
  case ERROR_FAILED_TO_RETRIEVE_CHALLENGER:
    PRINT_COLOR(COLOR_RED, "❌ Error: Could not retrieve the challenger.\n");
    break;
  case ERROR_FAILED_TO_HANDLE_CHALLENGE:
    PRINT_COLOR(COLOR_RED, "❌ Error: Failed to process the challenge.\n");
    break;
  case ERROR_LEAVE_GAME_NOT_RUNNING:
    PRINT_COLOR(COLOR_RED,
                "❌ Error: You cannot leave a game that isn’t running.\n");
    break;
  case ERROR_GET_GAME_NOT_RUNNING:
    PRINT_COLOR(COLOR_RED, "❌ Error: No running game to retrieve.\n");
    break;
  case ERROR_OBSRRVE_GAME_NOT_RUNNING:
    PRINT_COLOR(COLOR_RED, "❌ Error: No running game available to observe.\n");
    break;
  case ERROR_FAILED_TO_PLAY:
    PRINT_COLOR(COLOR_RED, "❌ Error: Failed to play the game.\n");
    break;
  case ERROR_PAGE_OUT_OF_BOUNDS:
    PRINT_COLOR(COLOR_RED, "❌ Error: The page is out of bounds.\n");
    break;
  default:
    PRINT_COLOR(COLOR_RED, "❌ Error: An unknown error occurred.\n");
    break;
  }

  return 1;
}

void add_error_handlers(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_ERROR_EVENT, &on_error_event);
}

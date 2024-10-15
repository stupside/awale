#include "error.h"

#include "lib/display/colors.h"
#include "lib/socket/cmd.h"
#include "lib/socket/cmds/error.h"

unsigned int on_error_event(unsigned int client_id, const void *data) {
  const struct ErrorEvent *event = data;

  switch (event->code) {
  case ERROR_CLIENT_NOT_FOUND:
    PRINT_COLOR(COLOR_RED, "Error: No availabble client was found\n");
    break;
  case ERROR_FAILED_TO_CHALLENGE:
    PRINT_COLOR(COLOR_RED, "Error: Failed to challenge\n");
    break;
  case ERROR_FAILED_TO_RETRIEVE_CHALLENGER:
    PRINT_COLOR(COLOR_RED, "Error: Failed to retrieve challenger\n");
    break;
  case ERROR_FAILED_TO_HANDLE_CHALLENGE:
    PRINT_COLOR(COLOR_RED, "Error: Failed to handle challenge\n");
    break;
  case ERROR_LEAVE_GAME_NOT_RUNNING:
    PRINT_COLOR(COLOR_RED, "Error: Leave game not running\n");
    break;
  case ERROR_GET_GAME_NOT_RUNNING:
    PRINT_COLOR(COLOR_RED, "Error: Get game not running\n");
    break;
  case ERROR_OBSRRVE_GAME_NOT_RUNNING:
    PRINT_COLOR(COLOR_RED, "Error: Observe game not running\n");
    break;
  default:
    PRINT_COLOR(COLOR_RED, "Error: Unknown error\n");
    break;
  }

  return 1;
};

void add_error_handlers(struct ServerMediator *mediator) {

  register_cmd(mediator, CMD_ERROR_EVENT, &on_error_event);
}
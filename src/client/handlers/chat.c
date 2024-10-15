#include "chat.h"

#include "lib/display/colors.h"
#include "lib/socket/cmd.h"
#include "lib/socket/cmds/chat.h"

unsigned int on_chat_write(unsigned int client_id, const void *data) {

  const struct ChatWriteEvent *event = data;

  PRINT_COLOR(COLOR_YELLOW, "User %d wrote: %s\n", event->client_id,
              event->message);

  return 1;
};

void add_chat_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_CHAT_WRITE_EVENT, &on_chat_write);
}
#include "user.h"

#include "lib/display/colors.h"
#include "lib/socket/cmd.h"
#include "lib/socket/cmds/user.h"

unsigned int on_user_register(unsigned int client_id, const void *data) {
  const struct UserRegisterRes *event = data;

  CLIENT_ID = event->client_id;

  PRINT_COLOR(COLOR_GREEN, "✅ You are registered! Your user ID is: %d\n",
              event->client_id);

  return 1;
}

unsigned int on_user_login(unsigned int client_id, const void *data) {

  PRINT_COLOR(COLOR_GREEN, "✅ You are logged in!\n");

  return 1;
}

unsigned int on_user_login_event(unsigned int client_id, const void *data) {
  const struct UserLoginEvent *event = data;

  PRINT_COLOR(COLOR_BLUE, "👤 User %d has logged in.\n", event->client_id);

  return 1;
}

unsigned int on_user_logout(unsigned int client_id, const void *data) {
  const struct UserLogoutEvent *event = data;

  PRINT_COLOR(COLOR_RED, "🚪 User %d has logged out.\n", event->client_id);

  return 1;
}

unsigned int on_user_list(unsigned int client_id, const void *data) {
  const struct UserListRes *res = data;

  PRINT_COLOR(COLOR_YELLOW, "📋 Online Users (%d):\n", res->count);

  for (unsigned int i = 0; i < res->count; i++) {
    printf("  👤 User %d: %s\n", res->users[i].client_id, res->users[i].name);
  }

  return 1;
}

unsigned int on_user_get_info(unsigned int client_id, const void *data) {
  const struct UserGetInfoRes *res = data;

  PRINT_COLOR(COLOR_CYAN, "🛠️ User Info:\n");
  PRINT_COLOR(COLOR_PURPLE, "  🔹 ID: %d\n", res->user.client_id);
  PRINT_COLOR(COLOR_PURPLE, "  🔹 Name: %s\n", res->user.name);
  PRINT_COLOR(COLOR_PURPLE, "  🔹 Description: %s\n", res->user.description);

  return 1;
}

void add_user_cmds(struct ServerMediator *mediator) {
  register_cmd(mediator, CMD_USER_LIST_ALL, &on_user_list, NO_PERSIST);
  register_cmd(mediator, CMD_USER_LOGOUT_EVENT, &on_user_logout, NO_PERSIST);
  register_cmd(mediator, CMD_USER_GET_INFO, &on_user_get_info, NO_PERSIST);
  register_cmd(mediator, CMD_USER_LOGIN, &on_user_login, NO_PERSIST);
  register_cmd(mediator, CMD_USER_REGISTER, &on_user_register, NO_PERSIST);
  register_cmd(mediator, CMD_USER_LOGIN_EVENT, &on_user_login_event,
               NO_PERSIST);
}

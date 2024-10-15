#include "cmd.h"

#include <string.h>

#define MAX_ARGS 15

unsigned int register_client_cmd(
    struct ClientMediator *mediator, const char prefix[MAX_PREFIX_LEN],
    unsigned int (*callback)(SOCKET sock, char *argv[], unsigned int argslen)) {

  struct ClientHandler handler = {.handle = callback};
  strcpy(handler.prefix, prefix);

  mediator->handlers[mediator->handler_c++] = handler;

  return 1;
}

unsigned int get_arguments(char *input, char *args[]) {
  int compteur = 0;

  char *token = strtok(input, " ");

  while (token != NULL && compteur < MAX_ARGS) {

    args[compteur++] = token;
    token = strtok(NULL, " ");
  }

  return compteur;
}

unsigned int handle_client_cmd(SOCKET sock,
                               const struct ClientMediator *mediator,
                               char *input) {
  char *args[MAX_ARGS];

  const unsigned argslen = get_arguments(input, args);

  for (int i = 0; i < mediator->handler_c; i++) {
    const struct ClientHandler *handler = &mediator->handlers[i];

    if (handler) {
      if (strcmp(args[0], handler->prefix) == 0) {
        return handler->handle(sock, args, argslen);
      }
    }
  }
  return 0;
}
#include "cmd.h"

#include <string.h>

unsigned int register_client_cmd(
    struct ClientMediator *mediator, char prefix[15],
    unsigned int (*callback)(SOCKET sock, char *argv[], unsigned int argslen)) {

  // Def of the handler
  struct ClientHandler handler;
  strcpy(handler.prefix, prefix);

  handler.handle = callback;

  static unsigned int cmd = 0;

  mediator->handlers[cmd++] = handler;

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

unsigned int handle_client_cmd(SOCKET sock, const struct ClientMediator *mediator,
                               char *input) {
  char *args[MAX_ARGS];

  const unsigned argslen = get_arguments(input, args);

  for (int i = 0; i < MAX_COMMANDS; i++) {

    struct ClientHandler handler = mediator->handlers[i];

    if (strcmp(args[0], handler.prefix) == 0) {
      return handler.handle(sock, args, argslen);
    }
  }
  return 0;
}
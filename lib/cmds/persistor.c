#include "persistor.h"
#include <string.h>

#define PATH_MODE "a+"

int open_persistor(Persistor *persistor, const char *path) {

  persistor->path = path;

  persistor->ptr = fopen(path, PATH_MODE);

  if (persistor->ptr == NULL) {

    perror("Failed to open file");
    return 0;
  }

  fseek(persistor->ptr, 0, SEEK_SET);

  return 1;
}

int close_persistor(Persistor *persistor) {

  if (fclose(persistor->ptr) == EOF) {

    perror("Failed to close file");
    return 0;
  }

  return 1;
}

int read_cmd(const Persistor *persistor, ClientId *client_id, char *cmd) {

  if (fscanf(persistor->ptr, "%d\n%s\n", client_id, cmd) < 0) {

    perror("Failed to read command");
    return 0;
  }

  return 1;
}

int write_cmd(const Persistor *persistor, ClientId client_id, const char *cmd) {

  char copy[strlen(cmd) + 1];

  strcpy(copy, cmd);

  strtok(copy, "\n");

  if (fprintf(persistor->ptr, "%d\n%s\n", client_id, cmd) < 0) {

    perror("Failed to write command");
    return 0;
  }

  fflush(persistor->ptr);

  return 1;
}
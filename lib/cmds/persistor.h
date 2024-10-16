#ifndef CMD_PERSISTOR_H
#define CMD_PERSISTOR_H

#include <stdio.h>

#include "lib/valuable/type.h"

typedef struct {
  FILE *ptr;
  const char *path;
} Persistor;

int open_persistor(Persistor *persistor, const char *path);

int close_persistor(Persistor *persistor);

int read_cmd(const Persistor *persistor, ClientId *client_id, char *cmd);
int write_cmd(const Persistor *persistor, ClientId client_id, const char *cmd);

#endif
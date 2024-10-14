#ifndef ERROR_H
#define ERROR_H

#include "lib/valuable/limit.h"

struct ErrorEvent {
  char message[ERROR_MSG_LEN];
};

#endif
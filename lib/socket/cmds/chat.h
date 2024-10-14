#ifndef CHAT_H
#define CHAT_H

#include "lib/valuable/limit.h"
#include "lib/valuable/type.h"

struct ChatWriteReq {
  char message[CHAT_MSG_LEN];
};

// struct ChatWriteRes {};
struct ChatWriteEvent {
  ClientId client_id;
  char message[CHAT_MSG_LEN];
};

#endif
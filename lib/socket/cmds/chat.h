#ifndef CHAT_H
#define CHAT_H

struct ChatWriteReq {
  char message[255];
};
// struct ChatWriteRes {};
struct ChatWriteEvent {
  char message[255];
  unsigned int client_id;
};

#endif
#ifndef CHAT_H

struct ChatWriteReq {
  char message[255];
};
// struct ChatWriteRes {};
struct ChatWriteEvent {
  char message[255];
  unsigned int client_id; // TODO: fix cmd, if client_id is before the message,
                          // the message is not received
};

#endif
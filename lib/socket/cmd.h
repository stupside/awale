#ifndef CMD_H
#define CMD_H

#include "pool.h"

#define MAX_COMMANDS 50

enum CMD {
  CMD_USER_INFO = 0x00,
  CMD_USER_LOGIN = 0x01,
  CMD_USER_LOGOUT = 0x02,

  CMD_USER_LIST_ALL = 0x03,
  CMD_USER_LIST_FRIENDS = 0x04,

  CMD_USER_ADD_FRIEND = 0x05,
  CMD_USER_REMOVE_FRIEND = 0x06,

  CMD_CHALLENGE = 0x07, // commander pour défier un joueur
  CMD_CHALLENGE_HANDLE = 0x08,

  CMD_GAME_PLAY = 0x09, // commande pour jouer un coup
  CMD_GAME_LEAVE = 0x0A,

  CMD_GAME_LIST = 0x0B,
  CMD_GAME_STATE = 0x0C,

  CMD_GAME_OBSERVE = 0x0D,

  CMD_CHAT_WRITE = 0x0E,

  CMD_ERROR = 0xFF,
};

#define CMD_ID_SIZE sizeof(char) * 2
#define PAYLOAD_LENGTH_SIZE sizeof(char) * 6

struct Handler {
  unsigned int (*handle)(unsigned int client_id, const void *data);
};

struct Mediator {
  struct Handler handlers[MAX_COMMANDS];
};

struct Mediator new_mediator();

void register_cmd(struct Mediator *mediator, enum CMD cmd,
                  unsigned int (*callback)(unsigned int client_id,
                                           const void *data));

char *inline_cmd(enum CMD cmd, const void *data, unsigned int data_size);

int compute_cmd(const struct Mediator *dispatcher, unsigned int client_id,
                const char *cmd, enum CMD *cmd_id);

#endif
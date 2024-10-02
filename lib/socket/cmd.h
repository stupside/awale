#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "b.h"
#include "socket.h"

#define MAX_COMMANDS 50

enum CMD {
  CMD_USER_INFO = 0x00,
  CMD_USER_LOGIN = 0x01,
  CMD_USER_LOGOUT = 0x02,

  CMD_USER_LIST_ALL = 0x03,
  CMD_USER_LIST_FRIENDS = 0x04,

  CMD_USER_ADD_FRIEND = 0x05,
  CMD_USER_REMOVE_FRIEND = 0x06,

  CMD_CHALLENGE = 0x07,
  CMD_CHALLENGE_ACCEPT = 0x08,
  CMD_CHALLENGE_REJECT = 0x09,

  CMD_GAME_PLAY = 0x0A,
  CMD_GAME_LEAVE = 0x0B,

  CMD_GAME_LIST = 0x0C,
  CMD_GAME_GRID = 0x0D,

  // CMD_GAME_OBSERVE = 0x0E,

  CMD_CHAT_WRITE = 0x0F,
};

#define CMD_ID_SIZE 2
#define PAYLOAD_LENGTH_SIZE 2

struct Handler {
  unsigned int (*handle)(const void *data);
};

struct Mediator {
  struct Handler handlers[MAX_COMMANDS];
};

struct Mediator new_mediator() {

  static struct Mediator mediator = {.handlers = {0}};

  return mediator;
}

void register_cmd(struct Mediator *mediator, enum CMD cmd,
                  unsigned int (*callback)(const void *data)) {

  mediator->handlers[cmd] = (struct Handler){.handle = callback};
}

int compute_cmd(const struct Mediator *dispatcher, const char *cmd) {

  char cmd_id_hex[CMD_ID_SIZE + 1];
  memcpy(cmd_id_hex, cmd, CMD_ID_SIZE);
  cmd_id_hex[CMD_ID_SIZE] = '\0';

  printf("CMD: %s\n", cmd_id_hex);

  char payload_len_hex[PAYLOAD_LENGTH_SIZE + 1];
  memcpy(payload_len_hex, cmd + CMD_ID_SIZE, PAYLOAD_LENGTH_SIZE);
  payload_len_hex[PAYLOAD_LENGTH_SIZE] = '\0';

  printf("Payload length: %s\n", payload_len_hex);

  const long payload_len = strtol(payload_len_hex, NULL, 16);

  char payload[payload_len + 1];
  memcpy(payload, cmd + CMD_ID_SIZE + PAYLOAD_LENGTH_SIZE, payload_len);
  payload[payload_len] = '\0';

  printf("Payload: %s\n", payload);

  const enum CMD id = strtol(cmd_id_hex, NULL, 16);

  const struct Handler handler = dispatcher->handlers[id];

  if (handler.handle == NULL) {
    printf("No handler for CMD %d\n", id);
    return 0;
  }

  void *data = malloc(payload_len);
  if (data == NULL) {
    perror("Failed to allocate memory for data");
    return -1;
  }

  byte_to_struct(payload, data, payload_len);

  int res = handler.handle(data);

  free(data);

  return res;
}

char *stringify_cmd(enum CMD cmd, const void *data, unsigned int size) {
  char cmd_hex[CMD_ID_SIZE + 1];
  char payload_len_hex[PAYLOAD_LENGTH_SIZE + 1];

  if (snprintf(cmd_hex, sizeof(cmd_hex), "%02X", cmd) < 0) {
    return NULL;
  }

  if (snprintf(payload_len_hex, sizeof(payload_len_hex), "%02X", size) < 0) {
    return NULL;
  }

  char *buffer = malloc(CMD_ID_SIZE + PAYLOAD_LENGTH_SIZE + size);
  if (buffer == NULL) {
    perror("Failed to allocate memory for buffer");
    return NULL;
  }

  snprintf(buffer, CMD_ID_SIZE + PAYLOAD_LENGTH_SIZE + 1, "%s%s", cmd_hex,
           payload_len_hex);

  char *payload = struct_to_byte(data, size);

  memcpy(buffer + CMD_ID_SIZE + PAYLOAD_LENGTH_SIZE, payload, size);

  free(payload);

  return buffer;
}

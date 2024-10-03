#include "cmd.h"
#include "b.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// Function to initialize a new Mediator
struct Mediator new_mediator() { return (struct Mediator){.handlers = {0}}; }

// Function to register a command with a callback
void register_cmd(struct Mediator *mediator, enum CMD cmd,
                  unsigned int (*callback)(unsigned int client_id,
                                           const void *data)) {
  mediator->handlers[cmd] = (struct Handler){.handle = callback};
}

int compute_cmd(const struct Mediator *dispatcher, unsigned int client_id,
                const char *cmd) {

  // Extract command ID from the received command
  char cmd_id_hex[CMD_ID_SIZE + 1];
  memcpy(cmd_id_hex, cmd, CMD_ID_SIZE);
  cmd_id_hex[CMD_ID_SIZE] = '\0';

  // Extract payload length from the received command
  char payload_len_hex[PAYLOAD_LENGTH_SIZE + 1];
  memcpy(payload_len_hex, cmd + CMD_ID_SIZE, PAYLOAD_LENGTH_SIZE);
  payload_len_hex[PAYLOAD_LENGTH_SIZE] = '\0';

  // Convert payload length from hexadecimal string to long
  const long payload_len = strtol(payload_len_hex, NULL, 16);

  // Allocate memory for the payload
  void *payload = malloc(payload_len);
  if (payload == NULL) {
    return 0; // Allocation failed
  }

  // Copy the data into the payload
  memcpy(payload, cmd + CMD_ID_SIZE + PAYLOAD_LENGTH_SIZE, payload_len);

  // Convert command ID from hexadecimal string to enum
  const enum CMD id = strtol(cmd_id_hex, NULL, 16);

  // Ensure the handler exists
  const struct Handler *handler = &dispatcher->handlers[id];
  if (handler == NULL) {
    free(payload); // Free allocated memory in case of error
    return 0;
  }

  // Call the handler with the payload data
  int res = handler->handle(client_id, payload);

  free(payload); // Free the memory after use

  return res;
}

char *inline_cmd(enum CMD cmd, const void *data, unsigned int data_size) {

  // Convert command enum to hexadecimal string
  char cmd_hex[CMD_ID_SIZE + 1];
  if (snprintf(cmd_hex, sizeof(cmd_hex), "%02X", cmd) < 0) {
    return NULL;
  }

  // Convert the data size to a hexadecimal string
  char payload_len_hex[PAYLOAD_LENGTH_SIZE + 1];
  if (snprintf(payload_len_hex, sizeof(payload_len_hex), "%02X", data_size) <
      0) {
    return NULL;
  }

  // Total buffer size
  unsigned int cmd_len = CMD_ID_SIZE + PAYLOAD_LENGTH_SIZE + data_size;

  // Allocate the buffer (command + payload length + data)
  char *buffer = malloc(cmd_len + 1);
  if (!buffer) {
    return NULL; // Check allocation
  }

  // Copy the command and the payload length into the buffer
  snprintf(buffer, CMD_ID_SIZE + PAYLOAD_LENGTH_SIZE + 1, "%s%s", cmd_hex,
           payload_len_hex);

  // Copy the raw struct data into the buffer (data may contain '\0')
  memcpy(buffer + CMD_ID_SIZE + PAYLOAD_LENGTH_SIZE, data, data_size);

  // Ensure the buffer is null-terminated (for safety)
  buffer[cmd_len] = '\0';

  return buffer; // Return the allocated buffer
}

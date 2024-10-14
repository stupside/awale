#ifndef CMDS_CMD_H
#define CMDS_CMD_H

typedef unsigned int CommandId;

int parse_cmd(const char *cmd, CommandId *cmd_id, unsigned char **cmd_payload);
char *inline_cmd(CommandId cmd_id, const void *data, unsigned int data_size);

#endif
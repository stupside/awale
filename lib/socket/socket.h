#ifndef SOCKET_H
#define SOCKET_H

#include "cmd.h"
#include "pool.h"

#define PORT 1977

#define CRLF "\r\n"

typedef struct in_addr IN_ADDR;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;

#define BUF_SIZE 1024

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

void close_socket(SOCKET sock);

int send_cmd_to(SOCKET sock, enum CMD cmd_id, const void *data,
                unsigned int data_size);

int send_cmd_to_all(const SocketPool *pool, const SocketClient *sender,
                    enum CMD cmd_id, const void *data, unsigned int data_size);

int read_from_socket(SOCKET sock, char *buffer);
#endif
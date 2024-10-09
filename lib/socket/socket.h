#ifndef SOCKET_H
#define SOCKET_H

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

int write_to_socket(SOCKET sock, const char *buffer);

int write_to_sockets(const SocketPool *pool, const SocketClient *sender,
                     const char *buffer);

int read_from_socket(SOCKET sock, char *buffer);
#endif
#include "main.h"

#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "lib/socket/cmd.h"
#include "lib/socket/cmds/user.h"
#include "lib/socket/socket.h"

SOCKET sock;
pthread_t read_thread, write_thread;

// Signal handler for SIGINT
void handle_sigint(int sig) {
  int ok = send_cmd_to(sock, CMD_USER_LOGOUT, NULL, 0);
  if (!ok) {
    perror("Failed to write to socket");
  }
  close_socket(sock);
  exit(0);
}

// Function to initialize the socket connection
int init(const char *address) {
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == INVALID_SOCKET) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  SOCKADDR_IN sin = {0};
  struct hostent *hostinfo = gethostbyname(address);
  if (hostinfo == NULL) {
    perror("Host not found");
    exit(EXIT_FAILURE);
  }

  sin.sin_addr = *(IN_ADDR *)hostinfo->h_addr;
  sin.sin_port = htons(PORT);
  sin.sin_family = AF_INET;

  if (connect(sock, (SOCKADDR *)&sin, sizeof(SOCKADDR)) == SOCKET_ERROR) {
    perror("Socket connection failed");
    close_socket(sock);
    exit(EXIT_FAILURE);
  }

  return sock;
}

// Function to handle reading from stdin and sending to the server
void *write_handler(void *clientMediator) {
  char buffer[BUF_SIZE];

  while (1) {
    // Read input from stdin
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
      if (buffer[0] == '\n')
        continue;                        // Ignore if empty
      buffer[strcspn(buffer, "\n")] = 0; // Remove newlines

      unsigned int res = handle_client_cmd(
          sock, (struct ClientMediator *)clientMediator, buffer);
      if (!res) {
        perror("Command not handled");
      }
    }
  }
  return NULL;
}

// Function to handle reading from the server
void *read_handler(void *mediator) {
  char buffer[BUF_SIZE];

  while (1) {
    if (read_from_socket(sock, buffer) == 0) {
      perror("Server down");
      break;
    }

    if (handle_cmd(mediator, -1, buffer)) {
      continue;
    } else {
      perror("Command %02X not handled");
    }
  }
  return NULL;
}

// Main application function
void app(const char *address, const char *name, const char *password,
         const struct ServerMediator *mediator,
         const struct ClientMediator *clientMediator) {
  sock = init(address);

  // Setup the signal handler for SIGINT
  signal(SIGINT, handle_sigint);

  // Send login command
  struct UserLoginReq req;
  strncpy(req.name, name, sizeof(req.name));
  strncpy(req.password, password, sizeof(req.password));

  send_cmd_to(sock, CMD_USER_LOGIN, &req, sizeof(struct UserLoginReq));

  // Create threads for reading and writing
  pthread_create(&write_thread, NULL, write_handler, (void *)clientMediator);
  pthread_create(&read_thread, NULL, read_handler, (void *)mediator);

  // Wait for the threads to finish (optional)
  pthread_join(write_thread, NULL);
  pthread_join(read_thread, NULL);

  // Send logout command when exiting
  int ok = send_cmd_to(sock, CMD_USER_LOGOUT, NULL, 0);
  if (!ok) {
    perror("Failed to write logout command to socket");
  }

  close_socket(sock);
}

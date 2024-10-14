#include "main.h"

#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "lib/socket/cmd.h"
#include "lib/socket/cmds/user.h"

#include "lib/socket/socket.h"

SOCKET sock;

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

// Main application function
void app(const char *address, const char *name, const char *password,
         const struct Mediator *mediator,
         const struct ClientMediator *clientMediator) {
  sock = init(address);
  char buffer[BUF_SIZE];
  fd_set rdfs;

  // Setup the signal handler for SIGINT
  signal(SIGINT, handle_sigint);

  {
    struct UserLoginReq req;
    strncpy(req.name, name, sizeof(req.name));
    strncpy(req.password, password, sizeof(req.password));

    send_cmd_to(sock, CMD_USER_LOGIN, &req, sizeof(struct UserLoginReq));
  }

  while (1) {
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs); // Add STDIN
    FD_SET(sock, &rdfs);         // Add the socket

    if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1) {
      close_socket(sock);
      exit(EXIT_FAILURE);
    }

    // If there's input from stdin (keyboard)
    if (FD_ISSET(STDIN_FILENO, &rdfs)) {
      if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Ignore if empty

        if (buffer[0] == '\n') {
          continue;
        }

        // Remove all newlines from the buffer
        buffer[strcspn(buffer, "\n")] = 0;

        {
          unsigned int res = handle_client_cmd(sock, clientMediator, buffer);

          if (!res) {
            perror("Command not handled");
          }
        }
      }
    } else if (FD_ISSET(sock, &rdfs)) {
      // Server down
      if (read_from_socket(sock, buffer) == 0) {
        perror("Server down");
        break;
      }

      if (handle_cmd(mediator, -1, buffer)) {
        // Command handled
      } else {
        perror("Command %02X not handled");
      }
    }
  }

  // Send logout command when exiting
  {
    const int ok = send_cmd_to(sock, CMD_USER_LOGOUT, NULL, 0);

    if (!ok) {
      perror("Failed to write logout command to socket");
    }
  }

  close_socket(sock);
}

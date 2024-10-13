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
  char *cmd = inline_cmd(CMD_USER_LOGOUT, NULL, 0);
  if (cmd) {
    int ok = write_to_socket(sock, cmd);
    if (!ok) {
      perror("Failed to write to socket");
    }
    free(cmd);
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
    fprintf(stderr, "Unknown host %s.\n", address);
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
void app(const char *address, const char *name, const struct Mediator *mediator,
         const struct ClientMediator *clientMediator) {
  sock = init(address);
  char buffer[BUF_SIZE];
  fd_set rdfs;

  // Setup the signal handler for SIGINT
  signal(SIGINT, handle_sigint);

  // Send the username to the server
  if (write_to_socket(sock, name) < 0) {
    perror("Failed to send name to server");
    close_socket(sock);
    exit(EXIT_FAILURE);
  }

  {
    struct UserLoginReq req;
    strncpy(req.name, name, sizeof(req.name) - 1);
    req.name[sizeof(req.name) - 1] = '\0'; // Ensure null-termination

    char *cmd = inline_cmd(CMD_USER_LOGIN, &req, sizeof(struct UserLoginReq));
    if (cmd) {
      write_to_socket(sock, cmd);
      free(cmd);
    } else {
      perror("Failed to create command for user login");
    }
  }

  while (1) {
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs); // Add STDIN
    FD_SET(sock, &rdfs);         // Add the socket

    if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1) {
      perror("Select failed");
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
            printf("This command does not seem to exist\n");
          }
        }
      } else if (FD_ISSET(sock, &rdfs)) {
        // Server down
        if (read_from_socket(sock, buffer) == 0) {
          printf("Server disconnected.\n");
          break;
        }

        if (!compute_cmd(mediator, -1, buffer)) {
          printf("Command not handled\n");
        }
      }
    }

    // Send logout command when exiting
    {
      char *cmd = inline_cmd(CMD_USER_LOGOUT, NULL, 0);
      if (cmd) {
        int ok = write_to_socket(sock, cmd);
        if (!ok) {
          perror("Failed to write logout command to socket");
        }
        free(cmd);
      }
    }

    close_socket(sock);
  }
}

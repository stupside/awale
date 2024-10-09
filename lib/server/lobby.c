#include "lobby.h"

#include <stdlib.h>

#include "lib/socket/cmd.h"
#include "lib/socket/cmds/challenge.h"
#include "lib/socket/socket.h"

struct Lobby new_lobby(struct SocketClient *p1, struct SocketClient *p2) {

  return (struct Lobby){
      .client = {p1, p2}, .awale = new_awale(), .state = LOBBY_STATE_WAITING};
}

int start_lobby(struct Lobby *lobby) {

  if (lobby->state == LOBBY_STATE_WAITING) {
    reset(&lobby->awale);
  } else {
    return 0;
  }

  lobby->state = LOBBY_STATE_PLAYING;

  return 1;
}

void end_lobby(struct Lobby *lobby) { lobby->state = LOBBY_STATE_FINISHED; }

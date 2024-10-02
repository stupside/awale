#pragma once

#include "lib/socket/cmd.h"

void init_mediator(struct Mediator *mediator) {

  register_cmd(mediator, CMD_CHALLENGE, NULL);

  register_cmd(mediator, CMD_CHALLENGE_ACCEPT, NULL);
  register_cmd(mediator, CMD_CHALLENGE_REJECT, NULL);
}

#pragma endregion
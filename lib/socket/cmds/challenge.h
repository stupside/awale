#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "lib/valuable/type.h"

struct ChallengeRes {};
struct ChallengeReq {
  ClientId client_id;
};
struct ChallengeEvent {
  ClientId client_id;
};

struct ChallengeHandleReq {
  unsigned int accept;
  ClientId client_id;
};
struct ChallengeHandleRes {};
struct ChallengeHandleEvent {
  unsigned int accept;
};

#endif
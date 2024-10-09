#ifndef CHALLENGE_H
#define CHALLENGE_H

struct ChallengeReq {
  unsigned int client_id;
};
// struct ChallengeRes {};
struct ChallengeEvent {
  unsigned int client_id;
};

struct ChallengeHandleReq {
  int accept;
};
// struct ChallengeHandleRes {};
struct ChallengeHandleEvent {
  int accept;
};

#endif
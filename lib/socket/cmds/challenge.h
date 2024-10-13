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
  unsigned int accept;
  unsigned int client_id;
};
// struct ChallengeHandleRes {};
struct ChallengeHandleEvent {
  unsigned int accept;
};

#endif
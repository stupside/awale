#pragma once

struct ChallengeReq {
  unsigned int client_id;
};
// struct ChallengeRes {};
struct ChallengeEvent {
  unsigned int client_id;
};

struct ChallengeAcceptReq {};
// struct ChallengeAcceptRes {};
struct ChallengeAcceptEvent {};

struct ChallengeDeclineReq {};
// struct ChallengeDeclineRes {};
struct ChallengeDeclineEvent {};

#pragma endregion
#ifndef TYPE_H
#define TYPE_H

typedef unsigned int ClientId;

enum Errors {
  ERROR_CLIENT_NOT_FOUND,
  ERROR_FAILED_TO_CHALLENGE,
  ERROR_FAILED_TO_RETRIEVE_CHALLENGER,
  ERROR_FAILED_TO_HANDLE_CHALLENGE,
  ERROR_LEAVE_GAME_NOT_RUNNING,
  ERROR_GET_GAME_NOT_RUNNING,
  ERROR_OBSRRVE_GAME_NOT_RUNNING,
  ERROR_OBSERVE_GAME_ERROR,
};

#endif
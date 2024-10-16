#ifndef CMD_H
#define CMD_H

enum CMD {

  CMD_USER_REGISTER,

  CMD_USER_LOGIN,
  CMD_USER_LOGIN_EVENT,

  CMD_USER_LOGOUT,
  CMD_USER_LOGOUT_EVENT,

  CMD_USER_SET_INFO,
  CMD_USER_GET_INFO,

  CMD_USER_LIST_ALL,
  CMD_USER_LIST_FRIENDS,

  CMD_USER_ADD_FRIEND,
  CMD_USER_REMOVE_FRIEND,

  CMD_CHALLENGE,
  CMD_CHALLENGE_EVENT,
  CMD_CHALLENGE_HANDLE,
  CMD_CHALLENGE_HANDLE_EVENT,

  CMD_GAME_PLAY,

  CMD_GAME_LEAVE,
  CMD_GAME_LEAVE_EVENT,

  CMD_GAME_LIST,
  CMD_GAME_STATE,

  CMD_GAME_OBSERVE,
  CMD_GAME_OBSERVE_EVENT,

  CMD_CHAT_WRITE,
  CMD_CHAT_WRITE_EVENT,

  CMD_GAME_STATE_EVENT,

  CMD_ERROR_EVENT,
};

#endif
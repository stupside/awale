#ifndef USER_H
#define USER_H

struct UserInfoReq {
  char name[255];
};
struct UserInfoRes {
  char description[255];
};

struct UserLoginReq {
  char name[255];
  char description[255];
};
struct UserLoginRes {
  unsigned int id;
};
struct UserLoginEvent {
  unsigned int id;
};

struct UserLogoutReq {};
// struct UserLogoutRes {};
struct UserLogoutEvent {
  unsigned int id;
};

#endif
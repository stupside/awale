#ifndef USER_H
#define USER_H

#include "lib/valuable/limit.h"
#include "lib/valuable/type.h"

struct UserLoginReq {
  char name[USER_NAME_LEN];
  char password[USER_PASSWORD_LEN];
  char description[USER_DESC_LEN];
};
struct UserLoginRes {
  ClientId client_id;
};
struct UserLoginEvent {
  ClientId id;
};

struct UserLogoutReq {};
struct UserLogoutEvent {
  ClientId client_id;
};

struct UserObserveReq {
  ClientId client_id;
};

struct UserListReq {
  unsigned int page;
};

struct UserRes {
  ClientId client_id;
  char name[USER_NAME_LEN];
  char description[USER_DESC_LEN];
};

#define PAGE_MAX_CLIENTS 2

struct UserListRes {
  unsigned int count;
  struct UserRes users[PAGE_MAX_CLIENTS];
};

struct UserSetInfoReq {
  char name[USER_NAME_LEN];
  char password[USER_PASSWORD_LEN];
  char description[USER_DESC_LEN];
};
struct UserSetInfoRes {};

struct UserGetInfoReq {};
struct UserGetInfoRes {
  struct UserRes user;
};
#endif
#ifndef USER_H
#define USER_H

#define PAGE_MAX_CLIENTS 10

struct UserInfoReq {
  char name[255];
};
struct UserInfoRes {
  char description[255];
};

struct UserLoginReq {
  char name[255];
  char password[255];
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

struct UserObserveReq {
  unsigned int id;
};

struct UserListReq {
  unsigned int page_number;
};

struct UserRes {
  unsigned int id;

  char name[255];
  char description[255];
};

struct UserListRes {
  unsigned int count; // <= PAGE_MAX_CLIENTS
  struct UserRes users[PAGE_MAX_CLIENTS];
};

#endif
#pragma once

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
// struct UserLoginRes {};

struct UserLogoutReq {
  char name[255];
};
// struct UserLogoutRes {};

#pragma endregion
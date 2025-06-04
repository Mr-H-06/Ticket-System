#ifndef USER_HPP
#define USER_HPP

#include "storage.cpp"

struct user_basic {
  //char username[20];   -->key
  char password[31];
  char name[21]; // Chinese
  char mailAddr[31];
  int privilege; //[0, 10]

  user_basic();

  bool operator==(const user_basic &other) const;

  bool operator<(const user_basic &other) const;

  bool operator!=(const user_basic &other) const;
};


class UserManager {
public:
  bool add_user(char *cur_username, char *username, user_basic &user);

  bool login(char *username, char *password);

  bool logout(char *username);

  bool query_profile(char *cur_username, char *username);

  bool modify_profile(char *cur_username, char *username, user_basic &user);

  void clear();

  UserManager();

private:
  BPlusTree<user_basic, 21, 50> basic;

  struct signed_in {
    char user[21];
    int privilege;
  };

  sjtu::vector<signed_in> log;
};
#endif

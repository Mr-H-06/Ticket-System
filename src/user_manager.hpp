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

  bool operator==(const user_basic &other) const {/*
    return strcmp(password, other.password) == 0 && strcmp(name, other.name) == 0 && strcmp(mailAddr, other.mailAddr) ==
           0 && privilege == other.privilege;*/
    return privilege == other.privilege;
    return false;
  }


  bool operator<(const user_basic &other) const {/*
    if (privilege != other.privilege) return privilege < other.privilege;
    if (strcmp(name, other.name) != 0) return strcmp(name, other.name) < 0;
    if (strcmp(mailAddr, other.mailAddr) != 0) return strcmp(mailAddr, other.mailAddr) < 0;
    return strcmp(password, other.password) < 0;*/
    return privilege < other.privilege;
    return false;
  }

  bool operator!=(const user_basic &other) const{
    return !(*this == other);
    return false;
  }
};


class UserManager {
public:
  bool addUser(char *cur_username, char *username, user_basic &user);

  bool login(char *username, char *password);

  bool logout(char *username);

  bool queryProfile(char *cur_username, char *username);

  bool modifyProfile(char *cur_username, char *username, user_basic &user);

  void clear();

  bool checkLogin(char *username);

  UserManager();

  BPlusTree<user_basic, 21, 35> basic;

  struct signed_in {
    char user[21];
    int privilege;
  };

  sjtu::vector<signed_in> log;
};
#endif

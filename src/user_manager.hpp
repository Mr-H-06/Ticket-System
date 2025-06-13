#ifndef USER_HPP
#define USER_HPP

#include "storage.cpp"

struct UserBasic {
  //char username[20];   -->key
  char password[31];
  char name[21]; // Chinese
  char mailAddr[31];
  int privilege; //[0, 10]

  UserBasic();

  bool operator==(const UserBasic &other) const {/*
    return strcmp(password, other.password) == 0 && strcmp(name, other.name) == 0 && strcmp(mailAddr, other.mailAddr) ==
           0 && privilege == other.privilege;*/
    return privilege == other.privilege;
    return false;
  }


  bool operator<(const UserBasic &other) const {/*
    if (privilege != other.privilege) return privilege < other.privilege;
    if (strcmp(name, other.name) != 0) return strcmp(name, other.name) < 0;
    if (strcmp(mailAddr, other.mailAddr) != 0) return strcmp(mailAddr, other.mailAddr) < 0;
    return strcmp(password, other.password) < 0;*/
    return privilege < other.privilege;
    return false;
  }

  bool operator!=(const UserBasic &other) const{
    return !(*this == other);
    return false;
  }
};


class UserManager {
public:
  bool addUser(char *cur_username, char *username, UserBasic &user);

  bool login(char *username, char *password);

  bool logout(char *username);

  bool queryProfile(char *cur_username, char *username);

  bool modifyProfile(char *cur_username, char *username, UserBasic &user);

  void clear();

  bool checkLogin(char *username);

  UserManager();

  BPlusTree<UserBasic, 21, 35> basic;

  struct SignedIn {
    char user[21];
    int privilege;
  };

  sjtu::vector<SignedIn> log;
};
#endif

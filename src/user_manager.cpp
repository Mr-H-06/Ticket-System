#include "user_manager.hpp"

user_basic::user_basic() {
  strcpy(password, "");
  strcpy(name, "");
  strcpy(mailAddr, "");
  privilege = -1;
}

bool user_basic::operator==(const user_basic &other) const {
  return strcmp(password, other.password) == 0 && strcmp(name, other.name) == 0 && strcmp(mailAddr, other.mailAddr) ==
         0 && privilege == other.privilege;
}

bool user_basic::operator<(const user_basic &other) const {
  return true;
}

bool UserManager::add_user(char *cur_username, char *username, user_basic &user) {
  if (basic.empty()) {
    user.privilege = 10;
    basic.insert(username, user);
    return true;
  }
  if (!basic.find(username).empty()) {
    return false;
  }
  for (int i = 0; i < log.size(); ++i) {
    if (strcmp(log[i].user, cur_username) == 0) {
      if (log[i].privilege > user.privilege) {
        basic.insert(username, user);
        return true;
      } else {
        return false;
      }
    }
  }
}

bool UserManager::login(char *username, char *password) {
  for (auto i: log) {
    if (strcmp(i.user, username)) return false;
  }
  auto find = basic.find(username);
  if (find.empty()) return false;
  if (strcmp(find[0].password, password) == 0) {
    log.push_back({username, find[0].privilege});
    return true;
  } else {
    return false;
  }
}

bool UserManager::logout(char *username) {
  for (int i = 0; i < log.size(); ++i) {
    if (strcmp(log[i].user, username) == 0) {
      log.erase(i);
      return true;
    }
  }
  return false;
}

bool UserManager::query_profile(char *cur_username, char *username) {
  for (int i = 0; i < log.size(); ++i) {
    if (strcmp(log[i].user, cur_username) == 0) {
      auto find = basic.find(username);
      if (!find.empty() && (log[i].privilege > find[0].privilege || strcmp(cur_username, username) == 0)) {
        std::cout << username << ' ' << find[0].name << ' ' << find[0].mailAddr << ' ' << find[0].privilege << '\n';
        return true;
      } else {
        return false;
      }
    }
  }
}

bool UserManager::modify_profile(char *cur_username, char *username, user_basic &user) {
  for (int i = 0; i < log.size(); ++i) {
    if (strcmp(log[i].user, cur_username) == 0) {
      auto find = basic.find(username);
      if (!find.empty() && (log[i].privilege > find[0].privilege || strcmp(cur_username, username) == 0)) {
        if (strcmp(user.name, "") == 0) {
          strcpy(user.name, find[0].name);
        }
        if (strcmp(user.password, "") == 0) {
          strcpy(user.password, find[0].password);
        }
        if (strcmp(user.mailAddr, "") == 0) {
          strcpy(user.mailAddr, find[0].password);
        }
        if (user.privilege == -1) {
          user.privilege = find[0].privilege;
        }
        basic.remove(username, find[0]);
        basic.insert(username, user);
        std::cout << username << ' ' << user.name << ' ' << user.mailAddr << ' ' << user.privilege << '\n';
        return true;
      } else {
        return false;
      }
    }
  }
}

UserManager::UserManager() : basic("user_basic.txt") {
}

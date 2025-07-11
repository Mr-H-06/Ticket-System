#include "user_manager.hpp"

UserBasic::UserBasic() {
  strcpy(password, "");
  strcpy(name, "");
  strcpy(mailAddr, "");
  privilege = -1;
}

bool UserManager::addUser(char *cur_username, char *username, UserBasic &user) {
  if (basic.empty()) {
    user.privilege = 10;
    basic.insert(username, user);
    return true;
  }
  if (!basic.find(username).empty()) {
    return false;
  }
  for (size_t i = 0; i < log.size(); ++i) {
    if (strcmp(log[i].user, cur_username) == 0) {
      if (log[i].privilege > user.privilege) {
        basic.insert(username, user);
        return true;
      }
      return false;
    }
  }
  return false;
}

bool UserManager::login(char *username, char *password) {
  for (auto i: log) {
    if (strcmp(i.user, username) == 0) return false;
  }
  auto find = basic.find(username);
  if (find.empty()) return false;
  if (strcmp(find[0].password, password) == 0) {
    SignedIn s;
    s.privilege = find[0].privilege;
    strcpy(s.user, username);
    log.push_back(s);
    return true;
  }
  return false;
}

bool UserManager::logout(char *username) {
  for (size_t i = 0; i < log.size(); ++i) {
    if (strcmp(log[i].user, username) == 0) {
      log.erase(i);
      return true;
    }
  }
  return false;
}

bool UserManager::queryProfile(char *cur_username, char *username) {
  for (size_t i = 0; i < log.size(); ++i) {
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
  return false;
}

bool UserManager::modifyProfile(char *cur_username, char *username, UserBasic &user) {
  for (size_t i = 0; i < log.size(); ++i) {
    if (strcmp(log[i].user, cur_username) == 0) {
      auto find = basic.find(username);
      auto cur = basic.find(cur_username);
      if (!find.empty() && (log[i].privilege > find[0].privilege || strcmp(cur_username, username) == 0) && cur[0].privilege > user.privilege) {
        if (strcmp(user.name, "") == 0) {
          strcpy(user.name, find[0].name);
        }
        if (strcmp(user.password, "") == 0) {
          strcpy(user.password, find[0].password);
        }
        if (strcmp(user.mailAddr, "") == 0) {
          strcpy(user.mailAddr, find[0].mailAddr);
        }
        if (user.privilege == -1) {
          user.privilege = find[0].privilege;
        }
        basic.remove(username, find[0]);
        basic.insert(username, user);
        std::cout << username << ' ' << user.name << ' ' << user.mailAddr << ' ' << user.privilege << '\n';
        return true;
      }
      return false;
    }
  }
  return false;
}

void UserManager::clear() {
  basic.clear();
  log.clear();
}

bool UserManager::checkLogin(char *username) {
  for (size_t i = 0; i < log.size(); ++i) {
    if (strcmp(log[i].user, username) == 0) {
      return true;
    }
  }
  return false;
}

UserManager::UserManager() : basic("UserBasic.txt") {
}

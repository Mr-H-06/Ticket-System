#include "storage.cpp"  //T, MAX_KEY_SIZE, MAX_KEY_PER_NODE
#include "order_manager.hpp"
#include "train_manager.hpp"
#include "user_manager.hpp"


int main() {
  std::ifstream file("/mnt/c/Users/hejia/Desktop/Ticket System/1867/1.in");
  std::string line;
  OrderManager order;
  UserManager user;
  TrainManager train;
  user.clear();
  order.clear();
  train.clear();

  while (std::getline(file, line)) {
    char *t = strtok(const_cast<char *>(line.c_str()), " ");
    std::cout << t << ' ';
    t = strtok(nullptr, " ");
    if (strcmp(t, "add_user") == 0) {
      user_basic u;
      char *cur_username, *username;
      t = strtok(nullptr, " ");
      while (t) {
        if (strcmp(t, "-c") == 0) {
          cur_username = strtok(nullptr, " ");
        } else if (strcmp(t, "-u") == 0) {
          username = strtok(nullptr, " ");
        } else if (strcmp(t, "-p") == 0) {
          strcpy(u.password, strtok(nullptr, " "));
        } else if (strcmp(t, "-n") == 0) {
          strcpy(u.name, strtok(nullptr, " "));
        } else if (strcmp(t, "-m") == 0) {
          strcpy(u.mailAddr, strtok(nullptr, " "));
        } else if (strcmp(t, "-g") == 0) {
          u.privilege = std::stoi(strtok(nullptr, " "));
        }
        t = strtok(nullptr, " ");
      }
      if (user.add_user(cur_username, username, u)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "login") == 0) {
      char *username, *password;
      t = strtok(nullptr, " ");
      while (t) {
        if (strcmp(t, "-u") == 0) {
          username = strtok(nullptr, " ");
        } else if (strcmp(t, "-p") == 0) {
          password = strtok(nullptr, " ");
        }
        t = strtok(nullptr, " ");;
      }
      if (user.login(username, password)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "logout") == 0) {
      t = strtok(nullptr, " ");
      if (strcmp(t, "-u") == 0) {
        char *username = strtok(nullptr, " ");
        if (user.logout(username)) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      }
    } else if (strcmp(t, "query_profile") == 0) {
      t = strtok(nullptr, " ");
      char *cur_username, *username;
      while (t) {
        if (strcmp(t, "-c") == 0) {
          cur_username = strtok(nullptr, " ");
        } else if (strcmp(t, "-u") == 0) {
          username = strtok(nullptr, " ");
        }
        t = strtok(nullptr, " ");
      }
      if (!user.query_profile(cur_username, username)) {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "modify_profile") == 0) {
      t = strtok(nullptr, " ");
      user_basic u;
      char *cur_username, *username;
      while (t) {
        if (strcmp(t, "-c") == 0) {
          cur_username = strtok(nullptr, " ");
        } else if (strcmp(t, " -u") == 0) {
          username = strtok(nullptr, " ");
        } else if (strcmp(t, "-p") == 0) {
          strcpy(u.password, strtok(nullptr, " "));
        } else if (strcmp(t, "-n") == 0) {
          strcpy(u.name, strtok(nullptr, " "));
        } else if (strcmp(t, "-m") == 0) {
          strcpy(u.mailAddr, strtok(nullptr, " "));
        } else if (strcmp(t, "-g") == 0) {
          u.privilege = std::stoi(strtok(nullptr, " "));
        }
        t = strtok(nullptr, " ");
      }
      if (!user.modify_profile(cur_username, username, u)) {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "add_train") == 0) {
      train_basic newtrain;
      char *trainId, *station_temp, *date_temp, *price_temp, *travelTime_temp, *stopoverTimes_temp;
      t = strtok(nullptr, " ");
      while (t) {
        if (strcmp(t, "-i") == 0) {
          trainId = strtok(nullptr, " ");
        } else if (strcmp(t, "-n") == 0) {
          newtrain.stationNum = std::stoi(strtok(nullptr, " "));
        } else if (strcmp(t, "-m") == 0) {
          newtrain.seatNum = std::stoi(strtok(nullptr, " "));
        } else if (strcmp(t, "-s") == 0) {
          station_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-p") == 0) {
          price_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-x") == 0) {
          strcpy(newtrain.startTime.hm, strtok(nullptr, " "));
        } else if (strcmp(t, "-t") == 0) {
          travelTime_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-o") == 0) {
          stopoverTimes_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-d") == 0) {
          date_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-y") == 0) {
          newtrain.type = strtok(nullptr, " ")[0];
        }
        t = strtok(nullptr, " ");
      }
      // station
      int ins = 0;
      t = strtok(station_temp, "|");
      while (t) {
        strcpy(newtrain.stations[ins++], t);
        t = strtok(nullptr, "|");
      }
      // saleDate
      strcpy(newtrain.saleDate[0].day, strtok(date_temp, "|"));
      strcpy(newtrain.saleDate[1].day, strtok(nullptr, "|"));
      // price
      ins = 0;
      t = strtok(price_temp, "|");
      while (t) {
        newtrain.price[ins++] = std::stoi(t);
        t = strtok(nullptr, "|");
      }
      // travalTime
      ins = 0;
      t = strtok(travelTime_temp, "|");
      while (t) {
        newtrain.travelTimes[ins++] = std::stoi(t);
        t = strtok(nullptr, "|");
      }
      // stopoverTime
      ins = 0;
      t = strtok(stopoverTimes_temp, "|");
      while (t) {
        newtrain.stopoverTimes[ins++] = std::stoi(t);
        t = strtok(nullptr, "|");
      }

      for (int i = 0; i < newtrain.stationNum - 2; ++i) {
        newtrain.stopoverTimes[i] += newtrain.travelTimes[i];
        newtrain.travelTimes[i + 1] += newtrain.stopoverTimes[i];
      }
      for (int i = newtrain.saleDate[1] - newtrain.saleDate[0]; i; --i) {
        for (int j = 0; j < newtrain.stationNum - 1; ++j) {
          newtrain.seat[i][j] = newtrain.seatNum;
        }
      }

      if (train.add_train(trainId, newtrain)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "delete_train") == 0) {
      t = strtok(nullptr, " ");
      if (strcmp(t, "-i") == 0) {
        t = strtok(nullptr, " ");
      }
      if (train.delete_train(t)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "release_train") == 0) {
      t = strtok(nullptr, " ");
      if (strcmp(t, "-i") == 0) {
        t = strtok(nullptr, " ");
      }
      if (train.release_train(t)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "query_train") == 0) {
      t = strtok(nullptr, " ");
      char *trainId;
      date d;
      while (t) {
        if (strcmp(t, "-i") == 0) {
          trainId = strtok(nullptr, " ");
        } else if (strcmp(t, "-d") == 0) {
          strcpy(d.day, strtok(nullptr, " "));
        }
        t = strtok(nullptr, " ");
      }
      if (!train.query_train(d, trainId)) {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "query_ticket") == 0) {
      t = strtok(nullptr, " ");
      bool type;
      date d;
      char *from, *to;
      while (t) {
        if (strcmp(t, "-s") == 0) {
          from = strtok(nullptr, " ");
        } else if (strcmp(t, "-t") == 0) {
          to = strtok(nullptr, " ");
        } else if (strcmp(t, "-d") == 0) {
          strcpy(d.day, strtok(nullptr, " "));
        } else if (strcmp(t, "-p") == 0) {
          t = strtok(nullptr, " ");
          if (strcmp(t, "time") == 0) {
            type = false;
          } else if (strcmp(t, "cost") == 0) {
            type = true;
          }
        }
        t = strtok(nullptr, " ");
      }
      train.query_ticket(d, from, to, type); //type = false -> time / true -> cost
    } else if (strcmp(t, "query_transfer") == 0) {
    } else if (strcmp(t, "buy_ticket") == 0) {
    } else if (strcmp(t, "query_order") == 0) {
    } else if (strcmp(t, "refund_ticket") == 0) {
    } else if (strcmp(t, "clean") == 0) {
      user.clear();
      train.clear();
      order.clear();
    } else if (strcmp(t, "exit") == 0) {
      std::cout << "bye\n";
      return 0;
    }
  }
}

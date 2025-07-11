#include "storage.cpp"  //T, MAX_KEY_SIZE, MAX_KEY_PER_NODE
#include "order_manager.hpp"
#include "train_manager.hpp"
#include "user_manager.hpp"
#include <cstdio>

int main() {

    /*
    std::cout << sizeof(Node<TrainBasic, 21, 13>) << '\n';
    std::cout << sizeof(Node<StationIdx, 41, 61>) << '\n';
    std::cout << sizeof(Node<UserBasic, 21, 35>) << '\n';
    std::cout << sizeof(Node<OrderBasic, 21, 24>) << '\n';
    std::cout << sizeof(Node<Waiting, 21, 27>) << '\n';
    std::cout << 4096 << ' ' << 4096 * 2 << ' ' << 4096 * 4 << ' ' << 4096 * 8 << ' ' << 4096 * 16;
    return 0;*/
  //clock_t start = clock();
  //double time_query_ticket = 0;
  //double time_query_profile = 0;
  //double time_buy_ticket = 0;
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  //std::ifstream file("/mnt/c/Users/hejia/Desktop/Ticket System/1867/33.in");
  //freopen("out.out", "w", stdout);
  std::string line;
  OrderManager order;
  UserManager user;
  TrainManager train;
  //user.clear();
  //order.clear();
  //train.clear();
  //while (std::getline(file, line)) {
  while (std::getline(std::cin, line)) {
    char *t = strtok(const_cast<char *>(line.c_str()), " ");
    std::cout << t << ' ';
    char idx[12];
    //memset(idx, 0, sizeof(idx));
    strcpy(idx, t);
    t = strtok(nullptr, " ");
    if (strcmp(t, "add_user") == 0) {
      UserBasic u;
      char cur_username[21], username[21];
      t = strtok(nullptr, " ");
      while (t) {
        if (strcmp(t, "-c") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(cur_username, strtok(nullptr, " "));
        } else if (strcmp(t, "-u") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(username, strtok(nullptr, " "));
        } else if (strcmp(t, "-p") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(u.password, strtok(nullptr, " "));
        } else if (strcmp(t, "-n") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(u.name, strtok(nullptr, " "));
        } else if (strcmp(t, "-m") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(u.mailAddr, strtok(nullptr, " "));
        } else if (strcmp(t, "-g") == 0) {
          //t = strtok(nullptr, " ");
          u.privilege = std::stoi(strtok(nullptr, " "));
        }
        t = strtok(nullptr, " ");
      }
      if (user.addUser(cur_username, username, u)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "login") == 0) {
      char username[21], password[31];
      t = strtok(nullptr, " ");
      while (t) {
        if (strcmp(t, "-u") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(username, strtok(nullptr, " "));
        } else if (strcmp(t, "-p") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(password, strtok(nullptr, " "));
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
        char username[21];
        //t = strtok(nullptr, " ");
        strcpy(username, strtok(nullptr, " "));
        if (user.logout(username)) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      }
    } else if (strcmp(t, "query_profile") == 0) {
      t = strtok(nullptr, " ");
      char cur_username[21], username[21];
      while (t) {
        if (strcmp(t, "-c") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(cur_username, strtok(nullptr, " "));
        } else if (strcmp(t, "-u") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(username, strtok(nullptr, " "));
        }
        t = strtok(nullptr, " ");
      }
      //clock_t a = clock();
      if (!user.queryProfile(cur_username, username)) {
        std::cout << "-1\n";
      }
      //clock_t b = clock();
      //time_query_profile += b - a;
    } else if (strcmp(t, "modify_profile") == 0) {
      t = strtok(nullptr, " ");
      UserBasic u;
      char cur_username[21], username[21];
      while (t) {
        if (strcmp(t, "-c") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(cur_username, strtok(nullptr, " "));
        } else if (strcmp(t, "-u") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(username, strtok(nullptr, " "));
        } else if (strcmp(t, "-p") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(u.password, strtok(nullptr, " "));
        } else if (strcmp(t, "-n") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(u.name, strtok(nullptr, " "));
        } else if (strcmp(t, "-m") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(u.mailAddr, strtok(nullptr, " "));
        } else if (strcmp(t, "-g") == 0) {
          //t = strtok(nullptr, " ");
          u.privilege = std::stoi(strtok(nullptr, " "));
        }
        t = strtok(nullptr, " ");
      }
      if (!user.modifyProfile(cur_username, username, u)) {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "add_train") == 0) {
      TrainBasic newtrain;
      Seats newtrain_seats;
      Station newtrain_stations;
      char trainId[21], station_temp[5000], date_temp[12], price_temp[600], travelTime_temp[500], stopoverTimes_temp[
        500];
      //memset(trainId, 0, 21);
      //memset(station_temp, 0, 5000);
      //memset(date_temp, 0, 12);
      //memset(price_temp, 0, 600);
      //memset(travelTime_temp, 0, 500);
      //memset(stopoverTimes_temp, 0, 500);
      t = strtok(nullptr, " ");
      while (t) {
        if (strcmp(t, "-i") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(trainId, strtok(nullptr, " "));
          //trainId = strtok(nullptr, " ");
        } else if (strcmp(t, "-n") == 0) {
          //t = strtok(nullptr, " ");
          newtrain.stationNum = std::stoi(strtok(nullptr, " "));
        } else if (strcmp(t, "-m") == 0) {
          //t = strtok(nullptr, " ");
          newtrain.seatNum = std::stoi(strtok(nullptr, " "));
        } else if (strcmp(t, "-s") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(station_temp, strtok(nullptr, " "));
          //station_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-p") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(price_temp, strtok(nullptr, " "));
          //price_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-x") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(newtrain.startTime, strtok(nullptr, " "));
        } else if (strcmp(t, "-t") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(travelTime_temp, strtok(nullptr, " "));
          //travelTime_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-o") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(stopoverTimes_temp, strtok(nullptr, " "));
          //stopoverTimes_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-d") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(date_temp, strtok(nullptr, " "));
          //date_temp = strtok(nullptr, " ");
        } else if (strcmp(t, "-y") == 0) {
          newtrain.type = strtok(nullptr, " ")[0];
        }
        t = strtok(nullptr, " ");
      }
      // station
      int ins = 0;
      t = strtok(station_temp, "|");
      while (t) {
        strcpy(newtrain_stations.stations[ins++], t);
        t = strtok(nullptr, "|");
      }
      // saleDate
      //t = strtok(date_temp, "|");
      //strcpy(newtrain.saleDate[0], strtok(date_temp, "|"));
      newtrain.saleDate[0] = Date(strtok(date_temp, "|"));
      //t = strtok(nullptr, "|");
      //strcpy(newtrain.saleDate[1], strtok(nullptr, "|"));
      newtrain.saleDate[1] = Date(strtok(nullptr, "|"));
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
      // Seats
      for (int i = Date(newtrain.saleDate[1]) - Date(newtrain.saleDate[0]); i >= 0; --i) {
        for (int j = 0; j < newtrain.stationNum - 1; ++j) {
          newtrain_seats.seat[i][j] = newtrain.seatNum;
        }
      }
      if (train.addTrain(trainId, newtrain, newtrain_seats, newtrain_stations)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "delete_train") == 0) {
      t = strtok(nullptr, " ");
      if (strcmp(t, "-i") == 0) {
        t = strtok(nullptr, " ");
      }
      if (train.deleteTrain(t)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "release_train") == 0) {
      t = strtok(nullptr, " ");
      if (strcmp(t, "-i") == 0) {
        t = strtok(nullptr, " ");
      }
      if (train.releaseTrain(t)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "query_train") == 0) {
      t = strtok(nullptr, " ");
      char trainId[21];
      Date d;
      while (t) {
        if (strcmp(t, "-i") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(trainId, strtok(nullptr, " "));
        } else if (strcmp(t, "-d") == 0) {
          //t = strtok(nullptr, " ");
          d = Date(strtok(nullptr, " "));
          //strcpy(d.day, strtok(nullptr, " "));
        }
        t = strtok(nullptr, " ");
      }
      if (!train.queryTrain(d, trainId)) {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "query_ticket") == 0) {
      t = strtok(nullptr, " ");
      bool type = false;
      Date d;
      char from[41], to[41];
      while (t) {
        if (strcmp(t, "-s") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(from, strtok(nullptr, " "));
        } else if (strcmp(t, "-t") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(to, strtok(nullptr, " "));
        } else if (strcmp(t, "-d") == 0) {
          //t = strtok(nullptr, " ");
          d = Date(strtok(nullptr, " "));
          //strcpy(d.day, strtok(nullptr, " "));
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
      //clock_t a = clock();
      train.queryTicket(d, from, to, type); //type = false -> time / true -> cost
      //clock_t b = clock();
      //time_query_ticket += b - a;
    } else if (strcmp(t, "query_transfer") == 0) {
      t = strtok(nullptr, " ");
      bool type = false;
      Date d;
      char from[41], to[41];
      while (t) {
        if (strcmp(t, "-s") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(from, strtok(nullptr, " "));
        } else if (strcmp(t, "-t") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(to, strtok(nullptr, " "));
        } else if (strcmp(t, "-d") == 0) {
          //t = strtok(nullptr, " ");
          d = Date(strtok(nullptr, " "));
          //strcpy(d.day, strtok(nullptr, " "));
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
      train.queryTransfer(d, from, to, type); //type = false -> time / true -> cost
    } else if (strcmp(t, "buy_ticket") == 0) {
      t = strtok(nullptr, " ");
      char username[21];
      OrderBasic neworder;
      neworder.idx = 0;
      for (size_t i = 1; i < strlen(idx) - 1; ++i) {
        neworder.idx *= 10;
        neworder.idx += idx[i] - '0';
      }
      bool type = false;
      while (t) {
        if (strcmp(t, "-u") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(username, strtok(nullptr, " "));
        } else if (strcmp(t, "-i") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(neworder.trainId, strtok(nullptr, " "));
        } else if (strcmp(t, "-d") == 0) {
          //t = strtok(nullptr, " ");
          neworder.leaving_time = DateTime(strtok(nullptr, " "), "00:00");
          //strcpy(neworder.leaving_time.date_.day, strtok(nullptr, " "));
        } else if (strcmp(t, "-n") == 0) {
          //t = strtok(nullptr, " ");
          neworder.num = std::stoi(strtok(nullptr, " "));
        } else if (strcmp(t, "-f") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(neworder.from, strtok(nullptr, " "));
        } else if (strcmp(t, "-t") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(neworder.to, strtok(nullptr, " "));
        } else if (strcmp(t, "-q") == 0) {
          t = strtok(nullptr, " ");
          if (t[0] == 't') {
            type = true;
          } else {
            type = false;
          }
        }
        t = strtok(nullptr, " ");
      }
      //clock_t a = clock();
      if (!order.buyTicket(username, neworder, type, train, user)) {
        std::cout << "-1\n";
      }
      //clock_t b = clock();
      //time_buy_ticket += b - a;
    } else if (strcmp(t, "query_order") == 0) {
      t = strtok(nullptr, " ");
      if (strcmp(t, "-u") == 0) {
        t = strtok(nullptr, " ");
      }
      if (!order.queryOrder(t, user)) {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "refund_ticket") == 0) {
      t = strtok(nullptr, " ");
      char username[41];
      int n = 1;
      while (t) {
        if (strcmp(t, "-u") == 0) {
          //t = strtok(nullptr, " ");
          strcpy(username, strtok(nullptr, " "));
        } else if (strcmp(t, "-n") == 0) {
          //t = strtok(nullptr, " ");
          n = std::stoi(strtok(nullptr, " "));
        }
        t = strtok(nullptr, " ");
      }
      if (order.refundTicket(username, n, user, train)) {
        std::cout << "0\n";
      } else {
        std::cout << "-1\n";
      }
    } else if (strcmp(t, "clean") == 0) {
      user.clear();
      train.clear();
      order.clear();
    } else if (strcmp(t, "exit") == 0) {
      std::cout << "bye\n";
      //clock_t end = clock();
      //std::cout << double(end - start) / CLOCKS_PER_SEC << '\n';
      //std::cout << "query_ticket:" << time_query_ticket / CLOCKS_PER_SEC << '\n';
      //std::cout << "query_profile:" << time_query_profile / CLOCKS_PER_SEC << '\n';
      //std::cout << "buy_ticket:" <<time_buy_ticket / CLOCKS_PER_SEC << '\n';
      return 0;
    }
  }
}

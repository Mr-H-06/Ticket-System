#include "train_manager.hpp"
#include "time_sys.hpp"

bool train_basic::operator<(const train_basic &other) const {
  if (stationNum != other.stationNum) return stationNum < other.stationNum;
  if (type != other.type) return type < other.type;
  if (seatNum != other.seatNum) return seatNum < other.seatNum;
  if (strcmp(startTime, other.startTime) != 0) return strcmp(startTime, other.startTime) < 0;
  if (strcmp(saleDate[0], other.saleDate[0]) != 0) return strcmp(saleDate[0], other.saleDate[0]) < 0;
  if (strcmp(saleDate[1], other.saleDate[1]) != 0) return strcmp(saleDate[1], other.saleDate[1]) < 0;
  if (release != other.release) return release < other.release;
  for (int i = 0; i < 100; ++i) {
    if (price[i] != other.price[i]){
      return price[i] < other.price[i];
    }
    if (stopoverTimes[i] != other.stopoverTimes[i]) {
      return stopoverTimes[i] < other.stopoverTimes[i];
    }
    if (travelTimes[i] != other.travelTimes[i]) {
      return travelTimes[i] < other.travelTimes[i];
    }
    if (strcmp(stations[i], other.stations[i]) != 0) {
      return strcmp(stations[i], other.stations[i]) < 0;
    }
  }
  return false;
}

bool train_basic::operator==(const train_basic &other) const {
  if (stationNum != other.stationNum) return false;
  if (type != other.type) return false;
  if (seatNum != other.seatNum) return false;
  if (strcmp(startTime, other.startTime) != 0) return false;
  if (strcmp(saleDate[0], other.saleDate[0]) != 0) return false;
  if (strcmp(saleDate[1], other.saleDate[1]) != 0) return false;
  if (release != other.release) return false;
  for (int i = 0; i < 100; ++i) {
    if (price[i] != other.price[i] || stopoverTimes[i] != other.stopoverTimes[i] || travelTimes[i] != other.travelTimes[i] || strcmp(stations[i], other.stations[i]) != 0) {
      return false;
    }
  }
  return true;
}

bool train_basic::operator!=(const train_basic &other) const {
  return !(*this == other);
}

bool seats::operator<(const seats &other) const {
  return true;
}

bool seats::operator==(const seats &other) const {
  return true;
}

bool seats::operator!=(const seats &other) const {
  return !(*this == other);
}

station_idx::station_idx() {
  strcpy(trainId, "");
}

station_idx::station_idx(char *trainId_) {
  strcpy(trainId, trainId_);
}


bool station_idx::operator<(const station_idx &other) const {
  return strcmp(trainId, other.trainId) < 0;
}

bool station_idx::operator==(const station_idx &other) const {
  return strcmp(trainId, other.trainId) == 0;
}

bool station_idx::operator!=(const station_idx &other) const {
  return !(*this == other);
}


bool TrainManager::add_train(char *trainId, train_basic &train, seats &train_seat) {
  if (!basic.find(trainId).empty()) return false;
  basic.insert(trainId, train);
  seat.insert(trainId, train_seat);
  return true;
}

bool TrainManager::delete_train(char *trainId) {
  auto find = basic.find(trainId);
  if (find.empty() || find[0].release == true) return false;
  basic.remove(trainId, find[0]);
  return true;
}

bool TrainManager::release_train(char *trainId) {
  auto find = basic.find(trainId);
  if (find.empty()) return false;
  basic.remove(trainId, find[0]);
  find[0].release = true;
  basic.insert(trainId, find[0]);
  auto Id = station_idx(trainId);
  for (auto x: find[0].stations) {
    station.insert(x, Id);
  }
  return true;
}

bool TrainManager::query_train(date d, char *trainId) {
  auto find = basic.find(trainId);
  auto s = seat.find(trainId);
  if (find.empty() || date(find[0].saleDate[1]) < d || d < date(find[0].saleDate[0])) return false;
  int del = d - date(find[0].saleDate[0]), cost = 0, stationnum = find[0].stationNum;
  date_time start(d, find[0].startTime), t;
  std::cout << trainId << ' ' << find[0].type << '\n';
  std::cout << find[0].stations[0] << " xx-xx xx:xx -> " << start.date_.day << ' ' << start.time_.hm << " 0 " <<
      s[0].seat[del][0];
  for (int i = 1; i < stationnum - 1; ++i) {
    t = start + find[0].travelTimes[i - 1];
    std::cout << find[0].stations[i] << ' ' << t.date_.day << ' ' << t.time_.hm << " -> ";
    t = start + find[0].stopoverTimes[i - 1];
    cost += find[0].price[i - 1];
    std::cout << t.date_.day << ' ' << t.time_.hm << ' ' << cost << ' ' << s[0].seat[del][i - 1] << '\n';
  }
  t = start + find[0].travelTimes[stationnum - 1];
  std::cout << find[0].stations[stationnum - 1] << ' ' << t.date_.day << ' ' << t.time_.hm << " -> xx-xx xx:xx " <<
      s[0].seat[del][stationnum - 1] << " x\n";
  return true;
}

void TrainManager::query_ticket(date d, char *from, char *to, bool type) {
  //type = false -> time / true -> cost
  auto find_from = station.find(from);
  auto find_to = station.find(to);
  if (find_from.empty() || find_to.empty()) {
    std::cout << "0\n";
  }
  if (type) {
    //cost
    sjtu::priority_queue<query_info, CompareCost> queue;
    query_info info;
    for (auto x: find_from) {
      for (auto y: find_to) {
        if (x == y) {
          auto find = basic.find(x.trainId);
          auto s = seat.find(x.trainId);
          int f, t;
          for (f = 0; f < find[0].stationNum; ++f) {
            if (strcmp(from, find[0].stations[f]) == 0) {
              break;
            }
          }
          for (t = 0; t < find[0].stationNum; ++t) {
            if (strcmp(to, find[0].stations[t]) == 0) {
              break;
            }
          }
          if (f > t) continue;
          strcpy(info.trainId, x.trainId);
          info.price = 0;
          int del = d - (date_time(find[0].saleDate[0], find[0].startTime) + find[0].travelTimes[f - 1]).date_;
          if (del < 0) {
            continue;
          }
          if ((date_time(find[0].saleDate[1], find[0].startTime) + find[0].travelTimes[t - 1]).date_ < d) {
            continue;
          }
          info.seat = s[0].seat[del][f];
          for (int k = f; k < t; ++k) {
            info.price += find[0].price[k];
            info.seat = std::min(info.seat, s[0].seat[del][k]);
          }
          if (f == 0) {
            info.time = find[0].travelTimes[t];
            info.leaving_time = date_time(d, find[0].startTime);
            info.arriving_time = info.leaving_time + find[0].travelTimes[t - 1];
          } else {
            info.time = find[0].travelTimes[t] - find[0].stopoverTimes[f];
            info.leaving_time = date_time(d, find[0].startTime) + find[0].stopoverTimes[f - 1];
            info.leaving_time.date_ = d;
            info.arriving_time = info.leaving_time + (find[0].travelTimes[t - 1] - find[0].stopoverTimes[f - 1]);
          }
          queue.push(info);
        }
      }
    }
    std::cout << queue.size() << '\n';
    while (!queue.empty()) {
      auto t = queue.top();
      queue.pop();
      std::cout << t.trainId << ' ' << from << ' ' << t.leaving_time.date_.day << ' ' << t.leaving_time.time_.hm <<
          " -> " << to << ' ' << t.arriving_time.date_.day << ' ' << t.arriving_time.time_.hm << ' ' << t.price << ' '
          << t.seat << '\n';
    }
  } else {
    //time
    sjtu::priority_queue<query_info, CompareTime> queue;
    query_info info;
    for (auto x: find_from) {
      for (auto y: find_to) {
        if (x == y) {
          auto find = basic.find(x.trainId);
          auto s = seat.find(x.trainId);
          int f, t;
          for (f = 0; f < find[0].stationNum; ++f) {
            if (strcmp(from, find[0].stations[f]) == 0) {
              break;
            }
          }
          for (t = 0; t < find[0].stationNum; ++t) {
            if (strcmp(to, find[0].stations[t]) == 0) {
              break;
            }
          }
          if (f > t) continue;
          strcpy(info.trainId, x.trainId);
          info.price = 0;
          int del = d - (date_time(find[0].saleDate[0], find[0].startTime) + find[0].travelTimes[f - 1]).date_;
          if (del < 0) {
            continue;
          }
          if ((date_time(find[0].saleDate[1], find[0].startTime) + find[0].travelTimes[t - 1]).date_ < d) {
            continue;
          }
          info.seat = s[0].seat[del][f];
          for (int k = f; k < t; ++k) {
            info.price += find[0].price[k];
            info.seat = std::min(info.seat, s[0].seat[del][k]);
          }
          if (f == 0) {
            info.time = find[0].travelTimes[t];
            info.leaving_time = date_time(d, find[0].startTime);
            info.arriving_time = info.leaving_time + find[0].travelTimes[t - 1];
          } else {
            info.time = find[0].travelTimes[t] - find[0].stopoverTimes[f];
            info.leaving_time = date_time(d, find[0].startTime) + find[0].stopoverTimes[f - 1];
            info.leaving_time.date_ = d;
            info.arriving_time = info.leaving_time + (find[0].travelTimes[t - 1] - find[0].stopoverTimes[f - 1]);
          }
          queue.push(info);
        }
      }
    }
    std::cout << queue.size() << '\n';
    while (!queue.empty()) {
      auto t = queue.top();
      queue.pop();
      std::cout << t.trainId << ' ' << from << ' ' << t.leaving_time.date_.day << ' ' << t.leaving_time.time_.hm <<
          " -> " << to << ' ' << t.arriving_time.date_.day << ' ' << t.arriving_time.time_.hm << ' ' << t.price << ' '
          << t.seat << '\n';
    }
  }
}

void TrainManager::query_transfer(date d, char *from, char *to, bool type) {
  auto find_from = station.find(from);
  auto find_to = station.find(to);
  if (find_from.empty() || find_to.empty()) {
    std::cout << "0\n";
    return;
  }
  sjtu::map<station_idx, sjtu::vector<transfer_info> > transfer;
  transfer_info info;
  for (auto x: find_from) {
    auto find = basic.find(x.trainId);
    auto s = seat.find(x.trainId);
    int f, k, del = d - date(find[0].saleDate[0]);
    for (f = 0; f < find[0].stationNum; ++f) {
      if (strcmp(x.trainId, find[0].stations[f]) == 0) {
        break;
      }
    }
    info.price = 0;
    info.seat = 1e9;
    if (f > 0) {
      info.leaving_time = date_time(d, find[0].startTime) + find[0].stopoverTimes[f - 1];
    } else {
      info.leaving_time = date_time(d, find[0].startTime);
    }
    for (k = f + 1; k < find[0].stationNum; ++k) {
      info.price += find[0].price[k - 1];
      info.seat = std::min(info.seat, s[0].seat[del][k - 1]);
      if (info.seat == 0) break;
      info.arriving_time = date_time(d, find[0].startTime) + find[0].travelTimes[k];
      transfer[x].push_back(info);
    }
  }


  int ans_time = 1e9, ans_price = 1e9;
  char trans_station[41];
  transfer_info first, second, present_second;
  if (type) {
    for (auto x: find_to) {
      // train from transfer station to final station
      present_second.price = 0;
      auto find = basic.find(x.trainId); // find the train
      int f, k;
      for (f = find[0].stationNum - 1; f; --f) {
        if (strcmp(find[0].stations[f], to) == 0) break; //find the station idx in the train
      }
      if (f == 0) continue;
      strcpy(present_second.trainId, x.trainId);
      for (k = f - 1; k > 0; --k) {
        present_second.price += find[0].price[k];
        auto it = transfer.find(find[0].stations[k]);
        //find if the present station can be arrived from start station
        if (it != transfer.end()) {
          for (auto y: it->second) {
            // from start -> transfer message
            present_second.leaving_time = date_time(find[0].saleDate[1], find[0].startTime) + find[0].stopoverTimes[
                                            k - 1];

            if (present_second.leaving_time < y.arriving_time) {
              continue;
            }
            if (y.arriving_time < date_time(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[k - 1]) {
              strcpy(present_second.leaving_time.date_.day, find[0].saleDate[0]);
            } else {
              strcpy(present_second.leaving_time.date_.day, y.arriving_time.date_.day);
            }
            present_second.arriving_time = present_second.leaving_time +
                                           (find[0].travelTimes[f - 1] - find[0].stopoverTimes[k - 1]);
            if (present_second.price + y.price > ans_price) {
              continue;
            }
            if (present_second.arriving_time - y.leaving_time > ans_time) {
              continue;
            }
            if (strcmp(y.trainId, first.trainId) > 0) {
              continue;
            }
            if (strcmp(present_second.trainId, second.trainId) > 0) {
              continue;
            }
            ans_time = present_second.arriving_time - y.leaving_time;
            first = y;
            strcpy(trans_station, find[0].stations[k]);
            ans_price = present_second.price + y.price;
            present_second.seat = 1e9;
            int del = present_second.leaving_time.date_ -
                      (date_time(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[k - 1]).date_;
            auto s = seat.find(x.trainId);
            for (int p = f - 1; p >= k; --p) {
              present_second.seat = std::min(present_second.seat, s[0].seat[del][p]);
            }
            second = present_second;
          }
        }
      }

      present_second.price += find[0].price[0];
      auto it = transfer.find(find[0].stations[0]);
      //find if the present station can be arrived from start station
      if (it != transfer.end()) {
        for (auto y: it->second) {
          // from start -> transfer message
          present_second.leaving_time = date_time(find[0].saleDate[1], find[0].startTime);

          if (present_second.leaving_time < y.arriving_time) {
            continue;
          }
          if (y.arriving_time < date_time(find[0].saleDate[0], find[0].startTime)) {
            strcpy(present_second.leaving_time.date_.day, find[0].saleDate[0]);
          } else {
            strcpy(present_second.leaving_time.date_.day, y.arriving_time.date_.day);
          }
          present_second.arriving_time = present_second.leaving_time + find[0].travelTimes[f - 1];
          if (present_second.price + y.price > ans_price) {
            continue;
          }
          if (present_second.arriving_time - y.leaving_time > ans_time) {
            continue;
          }
          if (strcmp(y.trainId, first.trainId) > 0) {
            continue;
          }
          if (strcmp(present_second.trainId, second.trainId) > 0) {
            continue;
          }
          ans_time = present_second.arriving_time - y.leaving_time;
          first = y;
          strcpy(trans_station, find[0].stations[0]);
          ans_price = present_second.price + y.price;
          present_second.seat = 1e9;
          int del = present_second.leaving_time.date_ - date_time(find[0].saleDate[0], find[0].startTime).date_;
          auto s = seat.find(x.trainId);
          for (int p = f - 1; p >= 0; --p) {
            present_second.seat = std::min(present_second.seat, s[0].seat[del][p]);
          }
          second = present_second;
        }
      }
    }
  } else {
    for (auto x: find_to) {
      // train from transfer station to final station
      present_second.price = 0;
      auto find = basic.find(x.trainId); // find the train
      int f, k;
      for (f = find[0].stationNum - 1; f; --f) {
        if (strcmp(find[0].stations[f], to) == 0) break; //find the station idx in the train
      }
      if (f == 0) continue;
      strcpy(present_second.trainId, x.trainId);
      for (k = f - 1; k > 0; --k) {
        present_second.price += find[0].price[k];
        auto it = transfer.find(find[0].stations[k]);
        //find if the present station can be arrived from start station
        if (it != transfer.end()) {
          for (auto y: it->second) {
            // from start -> transfer message
            present_second.leaving_time = date_time(find[0].saleDate[1], find[0].startTime) + find[0].stopoverTimes[
                                            k - 1];

            if (present_second.leaving_time < y.arriving_time) {
              continue;
            }
            if (y.arriving_time < date_time(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[k - 1]) {
              strcpy(present_second.leaving_time.date_.day, find[0].saleDate[0]);
            } else {
              strcpy(present_second.leaving_time.date_.day, y.arriving_time.date_.day);
            }
            present_second.arriving_time = present_second.leaving_time +
                                           (find[0].travelTimes[f - 1] - find[0].stopoverTimes[k - 1]);
            if (present_second.arriving_time - y.leaving_time > ans_time) {
              continue;
            }
            if (present_second.price + y.price > ans_price) {
              continue;
            }
            if (strcmp(y.trainId, first.trainId) > 0) {
              continue;
            }
            if (strcmp(present_second.trainId, second.trainId) > 0) {
              continue;
            }
            ans_time = present_second.arriving_time - y.leaving_time;
            first = y;
            strcpy(trans_station, find[0].stations[k]);
            ans_price = present_second.price + y.price;
            present_second.seat = 1e9;
            int del = present_second.leaving_time.date_ -
                      (date_time(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[k - 1]).date_;
            auto s = seat.find(x.trainId);
            for (int p = f - 1; p >= k; --p) {
              present_second.seat = std::min(present_second.seat, s[0].seat[del][p]);
            }
            second = present_second;
          }
        }
      }

      present_second.price += find[0].price[0];
      auto it = transfer.find(find[0].stations[0]);
      //find if the present station can be arrived from start station
      if (it != transfer.end()) {
        for (auto y: it->second) {
          // from start -> transfer message
          present_second.leaving_time = date_time(find[0].saleDate[1], find[0].startTime);

          if (present_second.leaving_time < y.arriving_time) {
            continue;
          }
          if (y.arriving_time < date_time(find[0].saleDate[0], find[0].startTime)) {
            strcpy(present_second.leaving_time.date_.day, find[0].saleDate[0]);
          } else {
            strcpy(present_second.leaving_time.date_.day, y.arriving_time.date_.day);
          }
          present_second.arriving_time = present_second.leaving_time + find[0].travelTimes[f - 1];
          if (present_second.arriving_time - y.leaving_time > ans_time) {
            continue;
          }
          if (present_second.price + y.price > ans_price) {
            continue;
          }
          if (strcmp(y.trainId, first.trainId) > 0) {
            continue;
          }
          if (strcmp(present_second.trainId, second.trainId) > 0) {
            continue;
          }
          ans_time = present_second.arriving_time - y.leaving_time;
          first = y;
          strcpy(trans_station, find[0].stations[0]);
          ans_price = present_second.price + y.price;
          present_second.seat = 1e9;
          int del = present_second.leaving_time.date_ - date_time(find[0].saleDate[0], find[0].startTime).date_;
          auto s = seat.find(x.trainId);
          for (int p = f - 1; p >= 0; --p) {
            present_second.seat = std::min(present_second.seat, s[0].seat[del][p]);
          }
          second = present_second;
        }
      }
    }
  }
  if (ans_price == 1e9) {
    std::cout << "0\n";
  } else {
    std::cout << first.trainId << ' ' << from << ' ' << first.leaving_time.date_.day << ' ' << first.leaving_time.time_.
        hm << " - > " << trans_station << ' ' << first.arriving_time.date_.day << ' ' << first.arriving_time.time_.hm <<
        ' ' << first.price << ' ' << first.seat << '\n';
    std::cout << second.trainId << ' ' << trans_station << ' ' << second.leaving_time.date_.day << ' ' << second.
        leaving_time.time_.
        hm << " - > " << to << ' ' << second.arriving_time.date_.day << ' ' << second.arriving_time.time_.hm <<
        ' ' << second.price << ' ' << second.seat << '\n';
  }
}

void TrainManager::clear() {
  basic.clear();
  seat.clear();
  station.clear();
}

TrainManager::TrainManager() : basic("train_basic.txt"), seat("train_seat.txt"), station("train_station.txt") {
}

#include "train_manager.hpp"
#include "time_sys.hpp"

bool train_basic::operator<(const train_basic &other) const {
  return true;
}

bool train_basic::operator==(const train_basic &other) const {
  return true;
}

bool train_basic::operator!=(const train_basic &other) const {
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


bool TrainManager::add_train(char *trainId, train_basic &train) {
  if (!basic.find(trainId).empty()) return false;
  basic.insert(trainId, train);
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
  if (find.empty() || date(find[0].saleDate[1]) < d || d < date(find[0].saleDate[0])) return false;
  int del = d - date(find[0].saleDate[0]), cost = 0, stationnum = find[0].stationNum;
  date_time start(d, find[0].startTime), t;
  std::cout << trainId << ' ' << find[0].type << '\n';
  std::cout << find[0].stations[0] << " xx-xx xx:xx -> " << start.date_.day << ' ' << start.time_.hm << " 0 " <<
      find[0].seat[0];
  for (int i = 1; i < stationnum - 1; ++i) {
    t = start + find[0].travelTimes[i - 1];
    std::cout << find[0].stations[i] << ' ' << t.date_.day << ' ' << t.time_.hm << " -> ";
    t = start + find[0].stopoverTimes[i - 1];
    cost += find[0].price[i - 1];
    std::cout << t.date_.day << ' ' << t.time_.hm << ' ' << cost << ' ' << find[0].seat[del][i - 1] << '\n';
  }
  t = start + find[0].travelTimes[stationnum - 1];
  std::cout << find[0].stations[stationnum - 1] << ' ' << t.date_.day << ' ' << t.time_.hm << " -> xx-xx xx:xx " <<
      find[0].seat[del][stationnum - 1] << " x\n";
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
          info.price = -find[0].price[t];
          int del = d - date(find[0].saleDate[0]);
          info.seat = find[0].seat[del][f];
          for (int k = f; k <= t; ++k) {
            info.price += find[0].price[k];
            info.seat = std::min(info.seat, find[0].seat[del][k]);
          }
          strcpy(info.trainId, x.trainId);
          info.arriving_time = date_time(d, find[0].startTime) + find[0].travelTimes[t];
          if (f == 0) {
            info.time = find[0].travelTimes[t];
            info.leaving_time = date_time(d, find[0].startTime);
          } else {
            info.time = find[0].travelTimes[t] - find[0].stopoverTimes[f];
            info.leaving_time = date_time(d, find[0].startTime) + find[0].stopoverTimes[f - 1];
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
          info.price = -find[0].price[t];
          int del = d - date(find[0].saleDate[0]);
          info.seat = find[0].seat[del][f];
          for (int k = f; k <= t; ++k) {
            info.price += find[0].price[k];
            info.seat = std::min(info.seat, find[0].seat[del][k]);
          }
          info.arriving_time = date_time(d, find[0].startTime) + find[0].travelTimes[t];
          if (f == 0) {
            info.time = find[0].travelTimes[t];
            info.leaving_time = date_time(d, find[0].startTime);
          } else {
            info.time = find[0].travelTimes[t] - find[0].stopoverTimes[f];
            info.leaving_time = date_time(d, find[0].startTime) + find[0].stopoverTimes[f - 1];
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
      info.seat = std::min(info.seat, find[0].seat[del][k - 1]);
      if (info.seat == 0) break;
      info.arriving_time = date_time(d, find[0].startTime) + find[0].travelTimes[k];
      transfer[x].push_back(info);
    }
  }


  if (type) {
  } else {
    int ans_ = 1e9, find_time;
    for (auto x: find_to) {
      int price = 0;
      auto find = basic.find(x.trainId);
      int f, k;
      for (f = find[0].stationNum - 1; f; --f) {
        if (strcmp(find[0].stations[f], to) == 0) break;
      }
      for (k = f - 1; k > 0; --k) {
        price += find[0].price[k];
        auto it = transfer.find(find[0].stations[k]);
        if (it != transfer.end()) {
          for (auto y: it->second) {
            if (date_time(find[0].saleDate[1], find[0].startTime) + find[0].stopoverTimes[k - 1] < y.arriving_time) {
              continue;
            }
            if (y.arriving_time < date_time(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[k - 1]) {

            }
          }
        }
      }
    }

  }
  std::cout << "-1\n";
}

void TrainManager::clear() {
  basic.clear();
  station.clear();
}

TrainManager::TrainManager() : basic("train_basic.txt"), station("train_station.txt") {
}

#include "train_manager.hpp"
#include "time_sys.hpp"

StationIdx::StationIdx() {
  strcpy(trainId, "");
}

StationIdx::StationIdx(char *trainId_) {
  strcpy(trainId, trainId_);
}

bool TrainManager::addTrain(char *trainId, TrainBasic &train, Seats &train_seat, Station&train_stations) {
  if (!basic.find(trainId).empty()) return false;
  train.seatAddr = seat.insert(train_seat);
  train.stationAddr = trainStations.insert(train_stations);
  basic.insert(trainId, train);
  //seat.insert(trainId, train_seat);
  return true;
}

bool TrainManager::deleteTrain(char *trainId) {
  auto find = basic.find(trainId);
  if (find.empty() || find[0].release == true) return false;
  basic.remove(trainId, find[0]);
  return true;
}

bool TrainManager::releaseTrain(char *trainId) {
  auto find = basic.find(trainId);
  if (find.empty() || find[0].release) return false;
  basic.remove(trainId, find[0]);
  find[0].release = true;
  basic.insert(trainId, find[0]);
  auto Id = StationIdx(trainId);
  Station train_station;
  trainStations.find(find[0].stationAddr, train_station);
  for (int x = 0; x < find[0].stationNum; ++x) {
    station.insert(train_station.stations[x], Id);
  }
  return true;
}

bool TrainManager::queryTrain(Date d, char *trainId) {
  auto find = basic.find(trainId);
  if (find.empty() || Date(find[0].saleDate[1]) < d || d < Date(find[0].saleDate[0])) return false;
  Seats s;
  seat.find(find[0].seatAddr, s);
  Station station;
  trainStations.find(find[0].stationAddr, station);
  int del = d - Date(find[0].saleDate[0]), cost = 0, stationnum = find[0].stationNum;
  DateTime start(d, find[0].startTime), t;
  std::cout << trainId << ' ' << find[0].type << '\n'; /*
  std::cout << find[0].stations[0] << " xx-xx xx:xx -> " << start.date_.day << ' ' << start.time_.hm << " 0 " <<
    s.seat[del][0] << '\n';*/
  std::cout << station.stations[0] << " xx-xx xx:xx -> " << start.toString() << " 0 " <<
      s.seat[del][0] << '\n';
  for (int i = 1; i < stationnum - 1; ++i) {
    t = start + find[0].travelTimes[i - 1];
    //std::cout << find[0].stations[i] << ' ' << t.date_.day << ' ' << t.time_.hm << " -> ";
    std::cout << station.stations[i] << ' ' << t.toString() << " -> ";
    t = start + find[0].stopoverTimes[i - 1];
    cost += find[0].price[i - 1];
    //std::cout << t.date_.day << ' ' << t.time_.hm << ' ' << cost << ' ' << s.seat[del][i] << '\n';
    std::cout << t.toString() << ' ' << cost << ' ' << s.seat[del][i] << '\n';
  }
  t = start + find[0].travelTimes[stationnum - 2]; /*
  std::cout << find[0].stations[stationnum - 1] << ' ' << t.date_.day << ' ' << t.time_.hm << " -> xx-xx xx:xx " <<
    cost + find[0].price[stationnum - 2] << " x\n";*/
  std::cout << station.stations[stationnum - 1] << ' ' << t.toString() << " -> xx-xx xx:xx " <<
      cost + find[0].price[stationnum - 2] << " x\n";
  return true;
}

void TrainManager::queryTicket(Date d, char *from, char *to, bool type) {
  //type = false -> time / true -> cost
  auto find_from = station.find(from);
  auto find_to = station.find(to);
  if (find_from.empty() || find_to.empty()) {
    std::cout << "0\n";
    return;
  }
  size_t i, j;
  QueryInfo info;
  sjtu::vector<QueryInfo> queue;
  Seats s;
  int cmp;
  for (i = 0, j = 0; i < find_from.size() && j < find_to.size();) {
    //auto x = find_from[0];
    //auto y = find_to[0];
    cmp = strcmp(find_from[i].trainId, find_to[j].trainId);
    if (cmp < 0) {
      ++i;
    } else if (cmp > 0) {
      ++j;
    } else {
      auto find = basic.find(find_from[i].trainId);
      Station station;
      trainStations.find(find[0].stationAddr, station);
      int f, t;
      for (f = 0; f < find[0].stationNum; ++f) {
        if (strcmp(from, station.stations[f]) == 0) {
          break;
        }
      }
      for (t = 0; t < find[0].stationNum; ++t) {
        if (strcmp(to, station.stations[t]) == 0) {
          break;
        }
      }
      if (f > t) {
        ++i;
        ++j;
        continue;
      }
      strcpy(info.trainId, find_from[i].trainId);
      info.price = 0;
      int del;
      if (f == 0) {
        //del = d - Date(find[0].saleDate[0]);
        del = d - Date(find[0].saleDate[0]);
      } else {
        //del = d - (DateTime(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[f - 1]).date_;
        del = d - (DateTime(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[f - 1]);
      }
      //if (del < 0 || (DateTime(find[0].saleDate[1], find[0].startTime) + find[0].stopoverTimes[f - 1]).date_ < d) {
      if (del < 0 || Date(DateTime(find[0].saleDate[1], find[0].startTime) + find[0].stopoverTimes[f - 1]) < d) {
        ++i;
        ++j;
        continue;
      }
      seat.find(find[0].seatAddr, s);
      info.seat = s.seat[del][f];
      for (int k = f; k < t; ++k) {
        info.price += find[0].price[k];
        info.seat = std::min(info.seat, s.seat[del][k]);
      }
      if (f == 0) {
        info.time = find[0].travelTimes[t - 1];
        info.leaving_time = DateTime(d, find[0].startTime);
        info.arriving_time = info.leaving_time + find[0].travelTimes[t - 1];
      } else {
        info.time = find[0].travelTimes[t - 1] - find[0].stopoverTimes[f - 1];
        info.leaving_time = DateTime(d, find[0].startTime) + find[0].stopoverTimes[f - 1];
        //info.leaving_time.date_ = d;
        info.leaving_time = d.day * 1440 + info.leaving_time.time % 1440;
        info.arriving_time = info.leaving_time + (find[0].travelTimes[t - 1] - find[0].stopoverTimes[f - 1]);
      }
      queue.push_back(info);
      ++i;
      ++j;
    }
  }
  if (type) {
    //cost
    quick_sort(queue, CompareCost());
  } else {
    //time
    quick_sort(queue, CompareTime());
  }
  std::cout << queue.size() << '\n';
  for (auto t: queue) {
    /*
        std::cout << t.trainId << ' ' << from << ' ' << t.leaving_time.date_.day << ' ' << t.leaving_time.time_.hm <<
            " -> " << to << ' ' << t.arriving_time.date_.day << ' ' << t.arriving_time.time_.hm << ' ' << t.price << ' '
        << t.seat << '\n';*/
    std::cout << t.trainId << ' ' << from << ' ' << t.leaving_time.toString() << " -> " << to << ' ' <<
        t.arriving_time.toString() << ' ' << t.price << ' ' << t.seat << '\n';
  }
}

void TrainManager::queryTransfer(Date d, char *from, char *to, bool type) {
  auto find_from = station.find(from);
  auto find_to = station.find(to);
  if (find_from.empty() || find_to.empty()) {
    std::cout << "0\n";
    return;
  }
  sjtu::map<std::string, sjtu::vector<TransferInfo> > transfer;
  for (auto x: find_from) {
    auto find = basic.find(x.trainId);
    Seats s;
    seat.find(find[0].seatAddr, s);
    Station station;
    trainStations.find(find[0].stationAddr, station);
    int f, k, del;
    for (f = 0; f < find[0].stationNum; ++f) {
      if (strcmp(from, station.stations[f]) == 0) {
        break;
      }
    }
    TransferInfo info;
    info.price = 0;
    info.seat = 1e9;
    strcpy(info.trainId, x.trainId);
    if (f > 0) {
      info.leaving_time = DateTime(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[f - 1];
      //del = d - info.leaving_time.date_;
      del = d - Date(info.leaving_time);
      //info.leaving_time.date_ = d;
      info.leaving_time = d.day * 1440 + info.leaving_time.time % 1440;
    } else {
      info.leaving_time = DateTime(d, find[0].startTime);
      del = d - Date(find[0].saleDate[0]);
    }
    if (del < 0 || del > Date(find[0].saleDate[1]) - Date(find[0].saleDate[0])) continue;
    for (k = f + 1; k < find[0].stationNum; ++k) {
      info.price += find[0].price[k - 1];
      info.seat = std::min(info.seat, s.seat[del][k - 1]);
      if (f == 0) {
        info.arriving_time = info.leaving_time + find[0].travelTimes[k - 1];
      } else {
        info.arriving_time = info.leaving_time + (find[0].travelTimes[k - 1] - find[0].stopoverTimes[f - 1]);
      }
      transfer[station.stations[k]].push_back(info);
    }
  }

  int ans_time = 1e9, ans_price = 1e9;
  char trans_station[41];
  TransferInfo first, second, present_second;
  if (type) {
    for (auto x: find_to) {
      // train from transfer station to final station
      present_second.price = 0;
      auto find = basic.find(x.trainId); // find the train
      Station station;
      trainStations.find(find[0].stationAddr, station);
      int t, k;
      for (t = find[0].stationNum - 1; t; --t) {
        if (strcmp(station.stations[t], to) == 0) break; //find the station idx in the train
      }
      if (t == 0) continue;
      strcpy(present_second.trainId, x.trainId);
      for (k = t - 1; k > 0; --k) {
        present_second.price += find[0].price[k];
        auto it = transfer.find(std::string(station.stations[k]));
        //find if the present station can be arrived from start station
        if (it != transfer.end()) {
          for (auto y: it->second) {
            // from start -> transfer message

            if (strcmp(y.trainId, x.trainId) == 0) continue;
            if (DateTime(find[0].saleDate[1], find[0].startTime) + find[0].stopoverTimes[k - 1] < y.arriving_time) {
              continue;
            }
            present_second.leaving_time = DateTime(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[
                                            k - 1];
            if (!(y.arriving_time < present_second.leaving_time)) {
              /*strcpy(present_second.leaving_time.date_.day, y.arriving_time.date_.day);
                if (strcmp(present_second.leaving_time.time_.hm, y.arriving_time.time_.hm) < 0) {
                present_second.leaving_time = present_second.leaving_time + 1440;
                }*/
              if (present_second.leaving_time.time % 1440 < y.arriving_time.time % 1440) {
                present_second.leaving_time = (Date(y.arriving_time).day + 1) * 1440 + present_second.leaving_time.time
                                              %
                                              1440;
              } else {
                present_second.leaving_time = Date(y.arriving_time).day * 1440 + present_second.leaving_time.time %
                                              1440;
              }
            }
            present_second.arriving_time = present_second.leaving_time +
                                           (find[0].travelTimes[t - 1] - find[0].stopoverTimes[k - 1]);
            if (present_second.price + y.price > ans_price) {
              continue;
            }
            if (present_second.price + y.price == ans_price) {
              if (present_second.arriving_time - y.leaving_time > ans_time) {
                continue;
              }
              if (present_second.arriving_time - y.leaving_time == ans_time) {
                if (strcmp(y.trainId, first.trainId) > 0) {
                  continue;
                }
                if (strcmp(y.trainId, first.trainId) == 0) {
                  if (strcmp(present_second.trainId, second.trainId) > 0) {
                    continue;
                  }
                }
              }
            }
            ans_time = present_second.arriving_time - y.leaving_time;
            first = y;
            strcpy(trans_station, station.stations[k]);
            ans_price = present_second.price + y.price;
            present_second.seat = 1e9; /*
            int del = present_second.leaving_time.date_ -
              (DateTime(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[k - 1]).date_;*/
            int del = Date(present_second.leaving_time) -
                      Date(DateTime(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[k - 1]);
            Seats s;
            seat.find(find[0].seatAddr, s);
            for (int p = t - 1; p >= k; --p) {
              present_second.seat = std::min(present_second.seat, s.seat[del][p]);
            }
            second = present_second;
          }
        }
      }

      present_second.price += find[0].price[0];
      auto it = transfer.find(station.stations[0]);
      //find if the present station can be arrived from start station
      if (it != transfer.end()) {
        for (auto y: it->second) {
          // from start -> transfer message

          if (strcmp(y.trainId, x.trainId) == 0) continue;
          if (DateTime(find[0].saleDate[1], find[0].startTime) < y.arriving_time) {
            continue;
          }
          present_second.leaving_time = DateTime(find[0].saleDate[0], find[0].startTime);
          if (!(y.arriving_time < present_second.leaving_time)) {
            /*strcpy(present_second.leaving_time.date_.day, y.arriving_time.date_.day);
            if (strcmp(present_second.leaving_time.time_.hm, y.arriving_time.time_.hm) < 0) {
              present_second.leaving_time = present_second.leaving_time + 1440;
            }*/
            if (present_second.leaving_time.time % 1440 < y.arriving_time.time % 1440) {
              present_second.leaving_time = (Date(y.arriving_time).day + 1) * 1440 + present_second.leaving_time.time %
                                            1440;
            } else {
              present_second.leaving_time = Date(y.arriving_time).day * 1440 + present_second.leaving_time.time %
                                            1440;
            }
          }
          present_second.arriving_time = present_second.leaving_time + find[0].travelTimes[t - 1];
          if (present_second.price + y.price > ans_price) {
            continue;
          }
          if (present_second.price + y.price == ans_price) {
            if (present_second.arriving_time - y.leaving_time > ans_time) {
              continue;
            }
            if (present_second.arriving_time - y.leaving_time == ans_time) {
              if (strcmp(y.trainId, first.trainId) > 0) {
                continue;
              }
              if (strcmp(y.trainId, first.trainId) == 0) {
                if (strcmp(present_second.trainId, second.trainId) > 0) {
                  continue;
                }
              }
            }
          }
          ans_time = present_second.arriving_time - y.leaving_time;
          first = y;
          strcpy(trans_station, station.stations[0]);
          ans_price = present_second.price + y.price;
          present_second.seat = 1e9;
          int del = Date(present_second.leaving_time) - Date(DateTime(find[0].saleDate[0], find[0].startTime));
          Seats s;
          seat.find(find[0].seatAddr, s);
          for (int p = t - 1; p >= 0; --p) {
            present_second.seat = std::min(present_second.seat, s.seat[del][p]);
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
      Station station;
      trainStations.find(find[0].stationAddr, station);
      int t, k;
      for (t = find[0].stationNum - 1; t; --t) {
        if (strcmp(station.stations[t], to) == 0) break; //find the station idx in the train
      }
      if (t == 0) continue;
      strcpy(present_second.trainId, x.trainId);
      for (k = t - 1; k > 0; --k) {
        present_second.price += find[0].price[k];
        auto it = transfer.find(std::string(station.stations[k]));

        if (it != transfer.end()) {
          for (auto y: it->second) {
            // from start -> transfer message

            if (strcmp(y.trainId, x.trainId) == 0) continue;
            if (DateTime(find[0].saleDate[1], find[0].startTime) + find[0].stopoverTimes[k - 1] < y.arriving_time) {
              continue;
            }
            present_second.leaving_time = DateTime(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[
                                            k - 1];

            if (!(y.arriving_time < present_second.leaving_time)) {
              /*strcpy(present_second.leaving_time.date_.day, y.arriving_time.date_.day);
              if (strcmp(present_second.leaving_time.time_.hm, y.arriving_time.time_.hm) < 0) {
                present_second.leaving_time = present_second.leaving_time + 1440;
              }*/
              if (present_second.leaving_time.time % 1440 < y.arriving_time.time % 1440) {
                present_second.leaving_time = (Date(y.arriving_time).day + 1) * 1440 + present_second.leaving_time.time
                                              %
                                              1440;
              } else {
                present_second.leaving_time = Date(y.arriving_time).day * 1440 + present_second.leaving_time.time %
                                              1440;
              }
            }
            present_second.arriving_time = present_second.leaving_time +
                                           (find[0].travelTimes[t - 1] - find[0].stopoverTimes[k - 1]);
            if (present_second.arriving_time - y.leaving_time > ans_time) {
              continue;
            }
            if (present_second.arriving_time - y.leaving_time == ans_time) {
              if (present_second.price + y.price > ans_price) {
                continue;
              }
              if (present_second.price + y.price == ans_price) {
                if (strcmp(y.trainId, first.trainId) > 0) {
                  continue;
                }
                if (strcmp(y.trainId, first.trainId) == 0) {
                  if (strcmp(present_second.trainId, second.trainId) > 0) {
                    continue;
                  }
                }
              }
            }
            ans_time = present_second.arriving_time - y.leaving_time;
            first = y;
            strcpy(trans_station, station.stations[k]);
            ans_price = present_second.price + y.price;
            present_second.seat = 1e9;
            int del = Date(present_second.leaving_time) -
                      Date(DateTime(find[0].saleDate[0], find[0].startTime) + find[0].stopoverTimes[k - 1]);
            Seats s;
            seat.find(find[0].seatAddr, s);
            for (int p = t - 1; p >= k; --p) {
              present_second.seat = std::min(present_second.seat, s.seat[del][p]);
            }
            second = present_second;
          }
        }
      }

      present_second.price += find[0].price[0];
      auto it = transfer.find(station.stations[0]);
      //find if the present station can be arrived from start station
      if (it != transfer.end()) {
        for (auto y: it->second) {
          // from start -> transfer message

          if (strcmp(y.trainId, x.trainId) == 0) continue;
          if (DateTime(find[0].saleDate[1], find[0].startTime) < y.arriving_time) {
            continue;
          }
          present_second.leaving_time = DateTime(find[0].saleDate[0], find[0].startTime);
          if (!(y.arriving_time < present_second.leaving_time)) {
            /*strcpy(present_second.leaving_time.date_.day, y.arriving_time.date_.day);
            if (strcmp(present_second.leaving_time.time_.hm, y.arriving_time.time_.hm) < 0) {
              present_second.leaving_time = present_second.leaving_time + 1440;
            }*/
            if (present_second.leaving_time.time % 1440 < y.arriving_time.time % 1440) {
              present_second.leaving_time = (Date(y.arriving_time).day + 1) * 1440 + present_second.leaving_time.time %
                                            1440;
            } else {
              present_second.leaving_time = Date(y.arriving_time).day * 1440 + present_second.leaving_time.time %
                                            1440;
            }
          }
          present_second.arriving_time = present_second.leaving_time + find[0].travelTimes[t - 1];
          if (present_second.arriving_time - y.leaving_time > ans_time) {
            continue;
          }
          if (present_second.arriving_time - y.leaving_time == ans_time) {
            if (present_second.price + y.price > ans_price) {
              continue;
            }
            if (present_second.price + y.price == ans_price) {
              if (strcmp(y.trainId, first.trainId) > 0) {
                continue;
              }
              if (strcmp(y.trainId, first.trainId) == 0) {
                if (strcmp(present_second.trainId, second.trainId) > 0) {
                  continue;
                }
              }
            }
          }
          ans_time = present_second.arriving_time - y.leaving_time;
          first = y;
          strcpy(trans_station, station.stations[0]);
          ans_price = present_second.price + y.price;
          present_second.seat = 1e9;
          //int del = present_second.leaving_time.date_ - DateTime(find[0].saleDate[0], find[0].startTime).date_;
          int del = Date(present_second.leaving_time) - Date(DateTime(find[0].saleDate[0], find[0].startTime));
          Seats s;
          seat.find(find[0].seatAddr, s);
          for (int p = t - 1; p >= 0; --p) {
            present_second.seat = std::min(present_second.seat, s.seat[del][p]);
          }
          second = present_second;
        }
      }
    }
  }
  if (ans_price == 1e9) {
    std::cout << "0\n";
  } else {
    std::cout << first.trainId << ' ' << from << ' ' << first.leaving_time.toString() << " -> " << trans_station << ' '
        << first.arriving_time.toString() << ' ' << first.price << ' ' << first.seat << '\n';
    std::cout << second.trainId << ' ' << trans_station << ' ' << second.leaving_time.toString() << " -> " << to << ' '
        << second.arriving_time.toString() << ' ' << second.price << ' ' << second.seat << '\n';
  }
}

void TrainManager::clear() {
  basic.clear();
  seat.clear();
  station.clear();
}

TrainManager::TrainManager() : basic("train_basic.txt"), seat("train_seat.txt"), trainStations("stationmemory.txt"), station("train_station.txt") {
}

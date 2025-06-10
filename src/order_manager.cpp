#include "order_manager.hpp"
#include "user_manager.hpp"

bool OrderManager::buy_ticket(char *username, order_basic &order, bool type, TrainManager &train, UserManager &user) {
  if (!user.check_log(username)) return false;
  auto find_train = train.basic.find(order.trainId);
  seats find_seat;
  train.seat.find(find_train[0].seatAddr, find_seat);
  if (find_train.empty() || !find_train[0].release) return false;
  int f, t;
  for (f = 0; f < find_train[0].stationNum; ++f) {
    if (strcmp(order.from, find_train[0].stations[f]) == 0) {
      break;
    }
  }
  for (t = f + 1; t < find_train[0].stationNum; ++t) {
    if (strcmp(order.to, find_train[0].stations[t]) == 0) {
      break;
    }
  }
  if (t >= find_train[0].stationNum) {
    return false;
  }
  int del;
  if (f > 0) {
    del = order.leaving_time.date_ - (date_time(find_train[0].saleDate[0], find_train[0].startTime) + find_train[0].
                                      stopoverTimes[f - 1]).date_;
    order.leaving_time.time_ = (date_time(order.leaving_time.date_, find_train[0].startTime) + find_train[0].
                                stopoverTimes[f - 1]).time_;
    order.arriving_time = order.leaving_time + (find_train[0].travelTimes[t - 1] - find_train[0].stopoverTimes[f - 1]);
  } else {
    del = order.leaving_time.date_ - find_train[0].saleDate[0];
    order.leaving_time.time_ = find_train[0].startTime;
    order.arriving_time = order.leaving_time + find_train[0].travelTimes[t - 1];
  }
  if (del < 0) return false;
  order.price = 0;
  for (int i = f; i < t; ++i) {
    order.price += find_train[0].price[i];
  }
  for (int i = f; i < t; ++i) {
    if (find_seat.seat[del][i] < order.num) {
      if (type) {
        waiting w;
        w.arriving_time = order.arriving_time;
        w.leaving_time = order.leaving_time;
        strcpy(w.from, order.from);
        strcpy(w.to, order.to);
        strcpy(w.username, username);
        w.idx = order.idx;
        w.num = order.num;
        waiting_queue.insert(order.trainId, w);
        strcpy(order.status, "pending");
        basic.insert(username, order);
        std::cout << "queue\n";
        return true;
      } else {
        return false;
      }
    }
  }
  strcpy(order.status, "success");
  //train.seat.remove(order.trainId, find_seat[0]);
  for (int i = f; i < t; ++i) {
    find_seat.seat[del][i] -= order.num;
  }
  train.seat.modify(find_train[0].seatAddr, find_seat);
  basic.insert(username, order);
  std::cout << order.price * order.num << '\n';
  return true;
}

bool OrderManager::query_order(char *username, UserManager &user) {
  if (!user.check_log(username)) {
    return false;
  }
  auto find = basic.find(username);
  std::cout << find.size() << '\n';
  for (auto x: find) {
    std::cout << '[' << x.status << "] " << x.trainId << ' ' << x.from << ' ' << x.leaving_time.date_.day << ' ' << x.
        leaving_time.time_.hm << " -> " << x.to << ' ' << x.arriving_time.date_.day << ' ' << x.arriving_time.time_.hm
        << ' ' << x.price << ' ' << x.num << '\n';
  }
  return true;
}


bool OrderManager::refund_ticket(char *username, int n, UserManager &user, TrainManager &train) {
  if (!user.check_log(username)) {
    return false;
  }
  auto find = basic.find(username);
  if (find.size() < n || strcmp(find[n - 1].status, "refunded") == 0) {
    return false;
  }
  if (strcmp(find[n - 1].status, "pending") == 0) {
    basic.remove(username, find[n - 1]);
    strcpy(find[n - 1].status, "refunded");
    basic.insert(username, find[n - 1]);
    auto waiting_list = waiting_queue.find(find[n - 1].trainId);
    for (auto x: waiting_list) {
      if (x.idx == find[n - 1].idx) {
        waiting_queue.remove(find[n - 1].trainId, x);
        return true;
      }
    }
    return true;
  }
  basic.remove(username, find[n - 1]);
  strcpy(find[n - 1].status, "refunded");
  basic.insert(username, find[n - 1]);
  auto find_train = train.basic.find(find[n - 1].trainId);
  seats seat;
  train.seat.find(find_train[0].seatAddr, seat);
  //train.seat.remove(find[n - 1].trainId, seat[0]);
  int f, t;
  for (f = 0; f < find_train[0].stationNum; ++f) {
    if (strcmp(find[n - 1].from, find_train[0].stations[f]) == 0) {
      break;
    }
  }
  for (t = f + 1; t < find_train[0].stationNum; ++t) {
    if (strcmp(find[n - 1].to, find_train[0].stations[t]) == 0) {
      break;
    }
  }
  int del;
  if (f > 0) {
    del = find[n - 1].leaving_time.date_ - (date_time(find_train[0].saleDate[0], find_train[0].startTime) + find_train[
                                              0].stopoverTimes[f - 1]).date_;
  } else {
    del = find[n - 1].leaving_time.date_ - find_train[0].saleDate[0];
  }
  for (int i = f; i < t; ++i) {
    seat.seat[del][i] += find[n - 1].num;
  }

  // deal with queue
  auto waiting_list = waiting_queue.find(find[n - 1].trainId);
  for (auto x: waiting_list) {
    for (f = 0; f < find_train[0].stationNum; ++f) {
      if (strcmp(x.from, find_train[0].stations[f]) == 0) {
        break;
      }
    }
    for (t = f + 1; t < find_train[0].stationNum; ++t) {
      if (strcmp(x.to, find_train[0].stations[t]) == 0) {
        break;
      }
    }
    if (f == 0) {
      del = x.leaving_time.date_ - find_train[0].saleDate[0];
    } else {
      del = x.leaving_time.date_ - (date_time(find_train[0].saleDate[0], find_train[0].startTime) + find_train[0].
                                    stopoverTimes[f - 1]).date_;
    }
    bool check = true;
    for (int i = f; i < t; ++i) {
      if (seat.seat[del][i] < x.num) {
        check = false;
        break;
      }
    }
    if (check) {
      waiting_queue.remove(find[n - 1].trainId, x);
      for (auto y: basic.find(x.username)) {
        if (y.idx == x.idx) {
          basic.remove(x.username, y);
          strcpy(y.status, "success");
          basic.insert(x.username, y);
          break;
        }
      }
      for (int i = f; i < t; ++i) {
        seat.seat[del][i] -= x.num;
      }
    }
  }

  train.seat.modify(find_train[0].seatAddr, seat);
  return true;
}

void OrderManager::clear() {
  basic.clear();
  waiting_queue.clear();
}

OrderManager::OrderManager() : basic("order.txt"), waiting_queue("waiting_queue.txt") {
}

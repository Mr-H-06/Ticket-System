#ifndef ORDER_HPP
#define ORDER_HPP

#include "storage.cpp"
#include "time_sys.hpp"
#include "train_manager.hpp"
#include "user_manager.hpp"

struct order_basic {
  // username - index
  int idx;
  char trainId[21];
  char status[11];
  char from[51];
  char to[51];
  date_time leaving_time;
  date_time arriving_time;
  int price;
  int num;

  bool operator<(const order_basic &other) const {
    return idx > other.idx;
  }

  bool operator==(const order_basic &other) const {
    return idx == other.idx;
  }

  bool operator!=(const order_basic &other) const {
    return idx != other.idx;
  }
};

struct waiting { //trainId
  int idx;
  char username[25];
  char from[51];
  char to[51];
  date_time leaving_time;
  date_time arriving_time;
  int num;

  bool operator<(const waiting &other)const {
    return idx < other.idx;
  }

  bool operator==(const waiting &other) const {
    return idx == other.idx;
  }

  bool operator!=(const waiting &other)const {
    return idx != other.idx;
  }
};

class OrderManager {
public:
  bool buy_ticket(char *username,order_basic &order, bool type, TrainManager &train, UserManager &user);

  bool query_order(char *username, UserManager &user);

  bool refund_ticket(char *username, int n, UserManager &user, TrainManager &train);

  void clear();

  OrderManager();

  BPlusTree<order_basic, 21, 50> basic;
  BPlusTree<waiting, 21, 50> waiting_queue;
};
#endif

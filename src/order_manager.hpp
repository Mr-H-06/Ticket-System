#ifndef ORDER_HPP
#define ORDER_HPP

#include "storage.cpp"
#include "time_sys.hpp"
#include "train_manager.hpp"
#include "user_manager.hpp"

struct OrderBasic {
  // username - index
  int idx;
  char trainId[21];
  char status[11];
  char from[41];
  char to[41];
  DateTime leaving_time;
  DateTime arriving_time;
  int price;
  int num;

  OrderBasic() = default;

  bool operator<(const OrderBasic &other) const {
    return idx > other.idx;
  }

  bool operator==(const OrderBasic &other) const {
    return idx == other.idx;
  }

  bool operator!=(const OrderBasic &other) const {
    return idx != other.idx;
  }
};

struct Waiting { //trainId
  int idx;
  char username[21];
  char from[41];
  char to[41];
  DateTime leaving_time;
  DateTime arriving_time;
  int num;

  Waiting() = default;

  bool operator<(const Waiting &other)const {
    return idx < other.idx;
  }

  bool operator==(const Waiting &other) const {
    return idx == other.idx;
  }

  bool operator!=(const Waiting &other)const {
    return idx != other.idx;
  }
};

class OrderManager {
public:
  bool buyTicket(char *username,OrderBasic &order, bool type, TrainManager &train, UserManager &user);

  bool queryOrder(char *username, UserManager &user);

  bool refundTicket(char *username, int n, UserManager &user, TrainManager &train);

  void clear();

  OrderManager();

  BPlusTree<OrderBasic, 21, 24> basic;
  BPlusTree<Waiting, 21, 27> waiting_queue;
};
#endif

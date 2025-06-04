#ifndef ORDER_HPP
#define ORDER_HPP

#include "storage.cpp"
#include "time_sys.hpp"
#include "user_manager.hpp"

struct order_basic {
};

class OrderManager {
public:
  bool buy_ticket(char *username, char *trainId, date d, int n, char *from, char *to, bool type);

  bool query_order(char *username, UserManager &user);

  bool refund_ticket(char *username, int n);

  void clear();

  OrderManager();

private:
  BPlusTree<order_basic, 21, 50> order;
};
#endif

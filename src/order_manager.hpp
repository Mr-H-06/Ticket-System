#ifndef ORDER_HPP
#define ORDER_HPP

#include "storage.cpp"

class OrderManager {
public:
  bool buy_ticket();

  bool query_order();

  bool refund_ticket();

  void clear();

  OrderManager() {
  }

private:
  //BPlusTree<>
};
#endif

#include "order_manager.hpp"
#include "user_manager.hpp"

bool OrderManager::buy_ticket(char *username, char *trainId, date d, int n, char *from, char *to, bool type) {
  return false;
}

bool OrderManager::query_order(char *username, UserManager &user) {
  return false;
}


bool OrderManager::refund_ticket(char *username, int n) {
  return false;
}

void OrderManager::clear() {
  order.clear();
}

OrderManager::OrderManager() : order("order.txt"){
}

#include "storage.cpp"  //T, MAX_KEY_SIZE, MAX_KEY_PER_NODE
#include "order_manager.hpp"
#include "train_manager.hpp"
#include "user_manager.hpp"


int main() {
  std::ifstream file("/mnt/c/Users/hejia/Desktop/Ticket System/1867/1.in");
  std::string read;
  OrderManager order;
  UserManager user;
  TrainManager train;

  char *line;
  while(std::getline(file, read)) {
    line = const_cast<char *>(read.c_str());
    char *t = strtok(const_cast<char *>(read.c_str()), " ");
    std::cout << t << ' ';
    while (t) {
      t = strtok(nullptr, " ");
    }
    std::cout << '\n';
  }
}

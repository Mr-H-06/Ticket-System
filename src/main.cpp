#include "storage.cpp"  //T, MAX_KEY_SIZE, MAX_KEY_PER_NODE

struct user_basic {
  //char username[20];   -->key
  char password[30];
  char name[20]; // Chinese
  char mailAddr[30];
  int privilege; //[0, 10]

  bool operator==(const user_basic &other) const {
    return strcmp(password, other.password) == 0 && strcmp(name, other.name) == 0 && strcmp(mailAddr, other.mailAddr) ==
           0 && privilege == other.privilege;
  }

  bool operator<(const user_basic &other) const {
    return true;
  }
};

struct train_basic {
  //char trainId[20];     -->key
  int stationNum;  //[0, 20];
  char stations[100][40]; //Chinese
  int seatNum; //   <1e5;
  int price
};

int main() {
  struct k {
    char s[10];

    bool operator==(const k &other) const {
      return strcmp(s, other.s) == 0;
    }

    bool operator<(const k &other) const {
      return strcmp(s, other.s) < 0;
    }
  };
  BPlusTree<k, 64, 170> bpt("bpt.txt");
  k t;
  strcpy(t.s, "哈");
  bpt.insert("你", t);
  strcpy(t.s, "eg");
  bpt.insert("你", t);
  strcpy(t.s, "ab的");
  bpt.insert("你", t);
  for (k i: bpt.find("你")) {
    std::cout << i.s << '\n';
  }
  std::string s = "我勒1个逗";
  std::cout << s;
}

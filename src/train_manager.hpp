#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "storage.cpp"
#include "time_sys.hpp"
#include "priority_queue.hpp"
#include <fstream>
#include "map.hpp"

struct train_basic {
  //char trainId[20];     -->key
  int stationNum; //[0, 20];
  char stations[100][51]; //Chinese
  int seatNum; //   <=1e5;
  int price[100]; // <=1e5
  char startTime[6]; // hh:mm
  int travelTimes[100]; // <=1e4       sum
  int stopoverTimes[100]; // <=1e4     sum
  char saleDate[2][6]; //start & end mm-dd
  char type;
  bool release;
  int seatAddr;

  train_basic() : release(false) {
  }

  bool operator<(const train_basic &other) const {
    /*
        if (stationNum != other.stationNum) return stationNum < other.stationNum;
        if (type != other.type) return type < other.type;
        if (seatNum != other.seatNum) return seatNum < other.seatNum;
        if (strcmp(startTime, other.startTime) != 0) return strcmp(startTime, other.startTime) < 0;
        if (strcmp(saleDate[0], other.saleDate[0]) != 0) return strcmp(saleDate[0], other.saleDate[0]) < 0;
        if (strcmp(saleDate[1], other.saleDate[1]) != 0) return strcmp(saleDate[1], other.saleDate[1]) < 0;
        if (release != other.release) return release < other.release;
        for (int i = 0; i < 100; ++i) {
          if (price[i] != other.price[i]) {
            return price[i] < other.price[i];
          }
          if (stopoverTimes[i] != other.stopoverTimes[i]) {
            return stopoverTimes[i] < other.stopoverTimes[i];
          }
          if (travelTimes[i] != other.travelTimes[i]) {
            return travelTimes[i] < other.travelTimes[i];
          }
          if (strcmp(stations[i], other.stations[i]) != 0) {
            return strcmp(stations[i], other.stations[i]) < 0;
          }
        }*/
    return false;
  }

  bool operator==(const train_basic &other) const {
    /*
        if (stationNum != other.stationNum) return false;
        if (type != other.type) return false;
        if (seatNum != other.seatNum) return false;
        if (strcmp(startTime, other.startTime) != 0) return false;
        if (strcmp(saleDate[0], other.saleDate[0]) != 0) return false;
        if (strcmp(saleDate[1], other.saleDate[1]) != 0) return false;
        if (release != other.release) return false;
        for (int i = 0; i < 100; ++i) {
          if (price[i] != other.price[i] || stopoverTimes[i] != other.stopoverTimes[i] || travelTimes[i] != other.
              travelTimes[i] || strcmp(stations[i], other.stations[i]) != 0) {
            return false;
          }
        }
        return true;*/
    return false;
  }

  bool operator!=(const train_basic &other) const {
    //return !(*this == other);
    return false;
  }
};

struct seats {
  int seat[100][30];

  seats() = default;

  bool operator<(const seats &other) const {
    return false;
  }

  bool operator==(const seats &other) const {
    return false;
  }

  bool operator!=(const seats &other) const {
    return false;
  }
};

struct station_idx {
  char trainId[21];

  station_idx();

  station_idx(char *trainId_);

  bool operator<(const station_idx &other) const {
    return strcmp(trainId, other.trainId) < 0;
  }

  bool operator==(const station_idx &other) const {
    return strcmp(trainId, other.trainId) == 0;
  }

  bool operator!=(const station_idx &other) const {
    return !(*this == other);
  }
};

class Block {
  std::fstream file;
  std::string filename;

  int allocate() {
    file.seekp(0, std::ios::end);
    int addr = file.tellp();
    seats newseat;
    file.write(reinterpret_cast<char *>(&newseat), sizeof(seats));
    return addr;
  }
public:
  Block(const std::string &fname) : filename(fname) {
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
      file.open(filename, std::ios::out | std::ios::binary);
      file.close();
      file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }
  }

  ~Block() {
    file.close();
  }

  int insert(seats &seat) {
    int addr = allocate();
    file.seekp(addr);
    file.write(reinterpret_cast<char *>(&seat), sizeof(seats));
    return addr;
  }

  void modify(int addr, seats &seat) {
    file.seekp(addr);
    file.write(reinterpret_cast<char *>(&seat), sizeof(seats));
  }

  void find(int addr, seats &seat) {
    file.seekg(addr);
    file.read(reinterpret_cast<char *>(&seat), sizeof(seats));
  }


  void clear() {
    file.close();
    file.open(filename, std::ios::out | std::ios::trunc |  std::ios::binary);
    file.close();
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
  }
};

class TrainManager {
public:
  bool add_train(char *trainId, train_basic &train, seats &train_seat);

  bool delete_train(char *trainId);

  bool release_train(char *trainId);

  bool query_train(date d, char *trainId);

  void query_ticket(date d, char *from, char *to, bool type); //type = false -> time / true -> cost

  void query_transfer(date d, char *from, char *to, bool type); // type = false -> time / true -> cost

  void clear();

  TrainManager();

  struct query_info {
    date_time leaving_time;
    date_time arriving_time;
    char trainId[21];
    int price;
    int time;
    int seat;
  };

  struct CompareTime {
    bool operator()(const query_info &a, const query_info &b) {
      if (a.time != b.time) return a.time > b.time;
      return strcmp(a.trainId, b.trainId) > 0;
    }
  };

  struct CompareCost {
    bool operator()(const query_info &a, const query_info &b) {
      if (a.price != b.price) return a.price > b.price;
      return strcmp(a.trainId, b.trainId) > 0;
    }
  };

  struct transfer_info {
    char trainId[21];
    int seat;
    int price;
    //int time;
    date_time leaving_time;
    date_time arriving_time;
  };

  BPlusTree<train_basic, 21, 50> basic;
  Block seat;
  BPlusTree<station_idx, 41, 50> station;
};
#endif

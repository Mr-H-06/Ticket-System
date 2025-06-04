#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "storage.cpp"
#include "time_sys.hpp"
#include "priority_queue.hpp"
#include "map.hpp"

struct train_basic {
  //char trainId[20];     -->key
  int stationNum; //[0, 20];
  char stations[100][41]; //Chinese
  int seatNum; //   <=1e5;
  int price[100]; // <=1e5
  char startTime[5]; // hh:mm
  int travelTimes[100]; // <=1e4       sum
  int stopoverTimes[100]; // <=1e4     sum
  char saleDate[2][5]; //start & end mm-dd
  char type;
  bool release;
  int seat[100][100];

  train_basic() : release(false) {
  }

  bool operator<(const train_basic &other) const;

  bool operator==(const train_basic &other) const;

  bool operator!=(const train_basic &other) const;
};

struct station_idx {
  char trainId[20];

  station_idx();

  station_idx(char *trainId_);

  bool operator<(const station_idx &other) const;

  bool operator==(const station_idx &other) const;

  bool operator!=(const station_idx &other) const;
};

class TrainManager {
public:
  bool add_train(char *trainId, train_basic &train);

  bool delete_train(char *trainId);

  bool release_train(char *trainId);

  bool query_train(date d, char *trainId);

  void query_ticket(date d, char *from, char *to, bool type); //type = false -> time / true -> cost

  void query_transfer(date d, char *from, char *to, bool type); // type = false -> time / true -> cost

  void clear();

  TrainManager();

private:
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
    int time;
    date_time leaving_time;
    date_time arriving_time;
  };

  BPlusTree<train_basic, 21, 50> basic;
  BPlusTree<station_idx, 41, 50> station;
};
#endif

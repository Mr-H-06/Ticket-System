#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "storage.cpp"
#include "time_sys.hpp"

struct train_basic {
  //char trainId[20];     -->key
  int stationNum; //[0, 20];
  char stations[100][40]; //Chinese
  int seatNum; //   <=1e5;
  int price[100]; // <=1e5
  struct time startTime; // hh:mm
  int travelTimes[100]; // <=1e4       sum
  int stopoverTimes[100]; // <=1e4     sum
  date saleDate[2]; //start & end mm-dd
  char type;
  bool release;
  int seat[100][100];

  bool operator<(const train_basic &other) const;

  bool operator==(const train_basic &other) const;
};

struct station_idx{
  char trainId[20];
  station_idx(char *trainId_);

  bool operator<(const station_idx &other) const;

  bool operator==(const station_idx &other) const;
};

class TrainManager {
public:
  bool add_train(char *trainId, train_basic &train);

  bool delete_train(char *trainId);

  bool release_train(char *trainId);

  bool query_train(date d, char *trainId);

  void query_ticket();

  void query_transfer();

  TrainManager();

private:
  BPlusTree<train_basic, 20, 50> basic;
  BPlusTree<station_idx, 40, 50> station;
};
#endif

#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "storage.cpp"

struct train_basic {
  //char trainId[20];     -->key
  int stationNum;  //[0, 20];
  char stations[100][40]; //Chinese
  int seatNum; //   <=1e5;
  int price[100];   // <=1e5
  char startTime[5]; // hh:mm
  int travelTiimes[100]; // <=1e4
  char saleDate[2][5];  //start & end mm-dd
  char type;

  bool operator<(const train_basic&other) const;
  bool operator==(const train_basic&other) const;
};

struct station_idx {
  char trainId[20];
  bool operator<(const station_idx&other) const;
  bool operator==(const station_idx&other) const;
};

class TrainManager {
public:
  int add_train(train_basic &train);
  int query_train();
  TrainManager();
private:
  BPlusTree<train_basic, 20, 50> basic;
  BPlusTree<station_idx, 40, 50> station;
};
#endif
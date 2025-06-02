#include "train_manager.hpp"
#include "time_sys.hpp"

bool train_basic::operator<(const train_basic &other) const {
  return true;
}

bool train_basic::operator==(const train_basic &other) const {
  return true;
}

station_idx::station_idx(char *trainId_){
  strcpy(trainId, trainId_);
}


bool station_idx::operator<(const station_idx &other) const {
  return strcmp(trainId, other.trainId) < 0;
}

bool station_idx::operator==(const station_idx &other) const {
  return strcmp(trainId, other.trainId) == 0;
}


bool TrainManager::add_train(char *trainId, train_basic &train) {
  if (!basic.find(trainId).empty()) return false;
  basic.insert(trainId, train);
  return true;
}

bool TrainManager::delete_train(char *trainId) {
  auto find = basic.find(trainId);
  if (find.empty() || find[0].release == true) return false;
  basic.remove(trainId, find[0]);
  return true;
}

bool TrainManager::release_train(char *trainId) {
  auto find = basic.find(trainId);
  if (find.empty()) return false;
  basic.remove(trainId, find[0]);
  find[0].release = true;
  basic.insert(trainId, find[0]);
  auto Id = station_idx(trainId);
  for (auto x : find[0].stations) {
    station.insert(x, Id);
  }
  return true;
}


bool TrainManager::query_train(date d, char *trainId) {
  auto find = basic.find(trainId);
  if (find.empty() || find[0].saleDate[1] < d || d < find[0].saleDate[0]) return false;
  int del = d - find[0].saleDate[0];
  date_time start(d, find[0].startTime);
  std::cout << trainId << ' ' << find[0].type << '\n';
  std::cout << find[0].stations[0] << ' ' << "xx-xx xx:xx -> " << start.date_.day << ' ' <<
  return true;
}

TrainManager::TrainManager() : basic("train_basic.txt"), station("train_station.txt") {
}

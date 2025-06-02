#include  "train_manager.hpp"

bool train_basic::operator<(const train_basic &other) const {
  return true;
}

bool train_basic::operator==(const train_basic &other) const {
  return true;
}

bool station_idx::operator<(const station_idx &other) const {
    return strcmp(trainId, other.trainId) < 0;
}

bool station_idx::operator==(const station_idx &other) const {
  return strcmp(trainId, other.trainId) == 0;
}


int TrainManager::add_train() {

}

int TrainManager::query_train() {
}

TrainManager::TrainManager() : basic("train_basic.txt"), station("train_station.txt") {
}

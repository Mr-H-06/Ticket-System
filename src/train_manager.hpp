#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "storage.cpp"
#include "time_sys.hpp"
#include "priority_queue.hpp"
#include <fstream>
#include "map.hpp"

struct TrainBasic {
  //char trainId[20];     -->key
  int stationNum; //[0, 20];
  //char stations[95][41]; //Chinese
  int stationAddr;
  int seatNum; //   <=1e5;
  int price[95]; // <=1e5
  char startTime[6]; // hh:mm
  int travelTimes[95]; // <=1e4       sum
  int stopoverTimes[95]; // <=1e4     sum
  //char saleDate[2][6]; //start & end mm-dd
  Date saleDate[2];
  char type;
  bool release;
  int seatAddr;

  TrainBasic() : release(false) {
    //memset(stations, 0, sizeof(stations));
    //memset(startTime, 0, sizeof(startTime));
    //memset(travelTimes, 0, sizeof(travelTimes));
    //memset(stopoverTimes, 0, sizeof(stopoverTimes));
    //memset(saleDate, 0, sizeof(saleDate));
  }

  bool operator<(const TrainBasic &other) const {
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
    return stationNum < other.stationNum;
    return false;
  }

  bool operator==(const TrainBasic &other) const {
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
    return stationNum == other.stationNum;
    return false;
  }

  bool operator!=(const TrainBasic &other) const {
    return !(*this == other);
    return false;
  }
};

struct Seats {
  int seat[92][23];

  Seats() {
    //memset(seat, 0, sizeof(seat));
  };

  bool operator<(const Seats &other) const {
    return seat[0][0] < other.seat[0][0];
    return false;
  }

  bool operator==(const Seats &other) const {
    return seat[0][0] == other.seat[0][0];
    return false;
  }

  bool operator!=(const Seats &other) const {
    return !(*this == other);
    return false;
  }
};

struct Station {
  char stations[95][41];

  Station() = default;

  bool operator<(const Station &other) const {
    return strcmp(stations[0], other.stations[0]) < 0;
  }

  bool operator==(const Station &other) const {
    return strcmp(stations[0], other.stations[0]) == 0;
  }

  bool operator!=(const Station &other) const {
    return !(*this == other);
  }
};

struct StationIdx {
  char trainId[21];

  StationIdx();

  StationIdx(char *trainId_);

  bool operator!=(const StationIdx &other) const {
    return !(*this == other);
  }

  bool operator==(const StationIdx &other) const {
    return strcmp(trainId, other.trainId) == 0;
  }

  bool operator<(const StationIdx &other) const {
    return strcmp(trainId, other.trainId) < 0;
  }
};

template<typename T>

class Block {
  std::fstream file;
  std::string filename;

  /*int allocate() {
    file.seekp(0, std::ios::end);
    int addr = file.tellp();
    Seats newseat;
    file.write(reinterpret_cast<char *>(&newseat), sizeof(Seats));
    return addr;
  }*/
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

  //template<typename T>
  int insert(T &read) {
    file.seekp(0, std::ios::end);
    int addr = file.tellp();
    file.seekp(addr);
    file.write(reinterpret_cast<char *>(&read), sizeof(T));
    return addr;
  }

  //template<typename T>
  void modify(int addr, T &read) {
    file.seekp(addr);
    file.write(reinterpret_cast<char *>(&read), sizeof(T));
  }

  //template<typename T>
  void find(int addr, T &read) {
    file.seekg(addr);
    file.read(reinterpret_cast<char *>(&read), sizeof(T));
  }


  void clear() {
    file.close();
    file.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    file.close();
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
  }
};

class TrainManager {
public:
  bool addTrain(char *trainId, TrainBasic &train, Seats &train_seat, Station &train_stations);

  bool deleteTrain(char *trainId);

  bool releaseTrain(char *trainId);

  bool queryTrain(Date d, char *trainId);

  void queryTicket(Date d, char *from, char *to, bool type); //type = false -> time / true -> cost

  void queryTransfer(Date d, char *from, char *to, bool type); // type = false -> time / true -> cost

  void clear();

  TrainManager();

  struct QueryInfo {
    DateTime leaving_time;
    DateTime arriving_time;
    char trainId[21];
    int price;
    int time;
    int seat;
  };

  struct CompareTime {
    bool operator()(const QueryInfo &a, const QueryInfo &b) {
      /*
            if (a.time != b.time) return a.time < b.time;
            return strcmp(a.trainId, b.trainId) < 0;*/
      return a.time < b.time || (a.time == b.time && strcmp(a.trainId, b.trainId) < 0);
    }
  };

  struct CompareCost {
    bool operator()(const QueryInfo &a, const QueryInfo &b) {
      /*
            if (a.price != b.price) return a.price < b.price;
            return strcmp(a.trainId, b.trainId) < 0;*/
      return a.price < b.price || (a.price == b.price && strcmp(a.trainId, b.trainId) < 0);
    }
  };

  template<typename T, typename Compare = std::less<T> >
  void quick_sort(sjtu::vector<T> &vec, Compare comp = Compare()) {
    if (vec.empty()) return;

    std::function<void(int, int)> sort_range = [&](int left, int right) {
      if (left >= right) return;

      T pivot = vec[(left + right) / 2];
      int i = left, j = right;

      while (i <= j) {
        while (comp(vec[i], pivot)) i++;
        while (comp(pivot, vec[j])) j--;
        if (i <= j) {
          std::swap(vec[i], vec[j]);
          ++i;
          --j;
        }
      }

      sort_range(left, j);
      sort_range(i, right);
    };

    sort_range(0, vec.size() - 1);
  }

  struct TransferInfo {
    char trainId[21];
    int seat;
    int price;
    //int time;
    DateTime leaving_time;
    DateTime arriving_time;
  };

  BPlusTree<TrainBasic, 21, 13> basic;
  Block<Seats> seat;
  Block<Station> trainStations;
  BPlusTree<StationIdx, 41, 61> station;
};
#endif

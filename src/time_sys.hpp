#ifndef TIME_SYS_HPP
#define TIME_SYS_HPP

#include <cstring>

struct date {
  char day[5];

  date(char *day_);

  int operator-(const date &minus) const;

  bool operator<(const date &other) const;
};

struct time {
  char hm[5];
  time(char *hm_);
};

struct date_time {
  date date_;
  time time_;

  date_time(date d, time t);

  date_time operator+(int add) const;
};

#endif

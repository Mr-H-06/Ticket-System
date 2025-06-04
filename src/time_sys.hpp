#ifndef TIME_SYS_HPP
#define TIME_SYS_HPP

#include <cstring>

struct date {
  char day[6];

  date();

  date(char *day_);

  int operator-(const date &minus) const;

  bool operator<(const date &other) const;
};

struct timing {
  char hm[6];

  timing();

  timing(char *hm_);
};

struct date_time {
  date date_;
  timing time_;

  bool operator<(const date_time &other) const;

  date_time();

  date_time(date d, timing t);

  date_time operator+(int add) const;

  int operator-(const date_time &other) const;
};

#endif

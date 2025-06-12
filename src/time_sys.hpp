#ifndef TIME_SYS_HPP
#define TIME_SYS_HPP

#include <cstring>
#include <string>

struct date_time;

struct date {
  //char day[6];
  friend struct date_time;
  int day;

  date();

  date(const char *day_);

  date(const date_time &d);

  int operator-(const date &minus) const;

  bool operator<(const date &other) const;

  bool operator==(const date &) const;
};

/*struct timing {
  //char hm[6];
  int hm;

  timing();

  timing(char *hm_);

  bool operator==(const timing &) const;
};*/

struct date_time {/*
  date date_;
  timing time_;*/
  int time;

  bool operator<(const date_time &other) const;

  date_time();

  //date_time(date d, timing t);

  date_time(const char *d, const char *t);

  date_time(date d, const char *t);

  date_time(int t);

  date_time operator+(int add) const;

  std::string to_string();

  int operator-(const date_time &other) const;
};

#endif

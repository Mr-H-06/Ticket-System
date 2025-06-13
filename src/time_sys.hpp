#ifndef TIME_SYS_HPP
#define TIME_SYS_HPP

#include <cstring>
#include <string>

struct DateTime;

struct Date {
  //char day[6];
  friend struct DateTime;
  int day;

  Date();

  Date(const char *day_);

  Date(const DateTime &d);

  int operator-(const Date &minus) const;

  bool operator<(const Date &other) const;

  bool operator==(const Date &) const;
};

/*struct timing {
  //char hm[6];
  int hm;

  timing();

  timing(char *hm_);

  bool operator==(const timing &) const;
};*/

struct DateTime {/*
  Date date_;
  timing time_;*/
  int time;

  bool operator<(const DateTime &other) const;

  DateTime();

  //DateTime(Date d, timing t);

  DateTime(const char *d, const char *t);

  DateTime(Date d, const char *t);

  DateTime(int t);

  DateTime operator+(int add) const;

  std::string toString();

  int operator-(const DateTime &other) const;
};

#endif

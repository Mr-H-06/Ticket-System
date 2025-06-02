#include "time_sys.hpp"

date::date(char *day_) {
  strcpy(day, day_);
}

int date::operator-(const date &minus) const {
  if (day[1] < minus.day[1]) {
    return -1;
  }
  if (day[1] == minus.day[1]) {
    return (day[3] - minus.day[3]) * 10 + day[4] - minus.day[4];
  }
  if (day[1] - minus.day[1] == 1) {
    if (day[1] == '7') {
      return day[3] * 10 + day[4] + ('3' - minus.day[3]) * 10 - minus.day[4];
    } else {
      return day[3] * 10 + day[4] + ('3' - minus.day[3]) * 10 + 1 - minus.day[4];
    }
  }
  if (day[1] - minus.day[1] == 2) {
    return 31 + day[3] * 10 + day[4] + ('3' - minus.day[3]) * 10 - minus.day[4];
  }
}

bool date::operator<(const date &other) const {
  return other - *this > 0;
}

time::time(char *hm_) {
  strcpy(hm, hm_);
}

date_time::date_time(date d, time t) : date_(d), time_(t) {
}

date_time date_time::operator+(int add) const {
  char ret1[5];
  ret1[2] = ':';
  //minute
  int s = time_.hm[3] * 10 + time_.hm[4] + add % 60;
  add /= 60;
  add += s / 60;
  s %= 60;
  ret1[4] = '0' + s % 10;
  ret1[3] = '0' + s / 10;
  //hour
  s = time_.hm[0] * 10 + time_.hm[1] + add % 24;
  add /= 24;
  add += s / 24;
  s %= 24;
  ret1[1] = '0' + s % 10;
  ret1[0] = '0' + s / 10;
  //day
  s = date_.day[3] * 10 + date_.day[4] + add;
  char ret2[5];
  ret2[0] = '0';
  ret2[2] = '-';
  if (date_.day[1] == '6') {
    if (s <= 30) {
      ret2[1] = '6';
    } else {
      s -= 30;
      ret2[1] = '7';
    }
  } else {
    if (s <= 31) {
      ret2[1] = date_.day[1];
    } else {
      s -= 31;
      ret2[1] = date_.day[1] + 1;
    }
  }
  ret2[3] = '0' + s / 10;
  ret2[4] = '0' + s % 10;
  return date_time(date(ret2), time(ret1));
}

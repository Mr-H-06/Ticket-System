#include "time_sys.hpp"

#include <ctime>

date::date() {
  day = 0;
}

date::date(const char *day_) {
  //strcpy(day, day_);
  day = (day_[3] - '0') * 10 + day_[4] - '0';
  if (day_[1] == '6') {
    return;
  } else if (day_[1] == '7') {
    day += 30;
  } else if (day_[1] == '8') {
    day += 61;
  } else if (day_[1] == '9') {
    day += 92;
  } else {
    day = 1e9;
  }
}

date::date(const date_time &d) : day(d.time / 1440) {
}


int date::operator-(const date &minus) const {
  return day - minus.day; /*
  if (day[1] < minus.day[1]) {
    return -1;
  }
  if (day[1] == minus.day[1]) {
    return (day[3] - minus.day[3]) * 10 + day[4] - minus.day[4];
  }
  if (day[1] - minus.day[1] == 1) {
    if (day[1] == '7') {
      return (day[3] - '0') * 10 + day[4] - '0' + ('3' - minus.day[3]) * 10 - minus.day[4] + '0';
    } else {
      return (day[3] - '0') * 10 + day[4] - '0' + ('3' - minus.day[3]) * 10 + 1 - minus.day[4] + '0';
    }
  }
  if (day[1] - minus.day[1] == 2) {
    if (day[1] == '8') {
      return 31 + (day[3] - '0') * 10 + day[4] - '0' + ('3' - minus.day[3]) * 10 - minus.day[4] + '0';
    } else {
      return 32 + (day[3] - '0') * 10 + day[4] - '0' + ('3' - minus.day[3]) * 10 - minus.day[4] + '0';
    }
  }
  return 62 + (day[3] - '0') * 10 + day[4] - '0' + ('3' - minus.day[3]) * 10 - minus.day[4] + '0';
*/
}

bool date::operator<(const date &other) const {
  return day < other.day;
  //return strcmp(this->day, other.day) < 0;
}

bool date::operator==(const date &other) const {
  return day == other.day;
}

/*timing::timing() {
  //strcpy(hm, "");
  hm = 0;
}

timing::timing(char *hm_) {
  //strcpy(hm, hm_);
  hm = (hm_[0] - '0') * 600 + (hm_[1] - '0') *  60 + (hm_[3] - '0') * 10 + (hm_[4] - '0');
}

bool timing::operator<(const timing &other) const {
  return hm < other.hm;
}*/

bool date_time::operator<(const date_time &other) const {
  return time < other.time;
  /*int cmp = strcmp(this->date_.day, other.date_.day);
  if (cmp != 0) return cmp < 0;
  return strcmp(this->time_.hm, other.time_.hm) < 0;*/
}

date_time::date_time() {
  //memset(date_.day, 0, sizeof(date_));
  //memset(time_.hm, 0, sizeof(time_));
}

/*date_time::date_time(date d, timing t) : date_(d), time_(t) {
}*/

date_time::date_time(const char *d, const char *t = "00:00") {
  time = date(d).day * 1440 + (t[0] - '0') * 600 + (t[1] - '0') * 60 + (t[3] - '0') * 10 + (t[4] - '0');
}

date_time::date_time(date d, const char *t = "00:00") {
  time = d.day * 1440 + (t[0] - '0') * 600 + (t[1] - '0') * 60 + (t[3] - '0') * 10 + (t[4] - '0');
}

date_time::date_time(int t) : time(t) {
}


date_time date_time::operator+(int add) const {
  return time + add; /*
  char ret1[6];
  ret1[2] = ':';
  //minute
  int s = (time_.hm[3] - '0') * 10 + (time_.hm[4] - '0') + add % 60;
  add /= 60;
  add += s / 60;
  s %= 60;
  ret1[4] = '0' + s % 10;
  ret1[3] = '0' + s / 10;
  //hour
  s = (time_.hm[0] - '0') * 10 + (time_.hm[1] - '0') + add % 24;
  add /= 24;
  add += s / 24;
  s %= 24;
  ret1[1] = '0' + s % 10;
  ret1[0] = '0' + s / 10;
  //day
  s = (date_.day[3] - '0') * 10 + (date_.day[4] - '0') + add;
  char ret2[6];
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
  ret1[5] = ret2[5] = '\0';
  return date_time(date(ret2), timing(ret1));*/
}

std::string date_time::to_string() {
  int tmp = time, deal;
  std::string s;
  deal = tmp % 60;
  s = ":" + std::to_string(deal / 10) + std::to_string(deal % 10);
  tmp /= 60;
  deal = tmp % 24;
  s = " " + std::to_string(deal / 10) + std::to_string(deal % 10) + s;
  tmp /= 24;
  if (tmp <= 30) {
    s = "06-" + std::to_string(tmp / 10) + std::to_string(tmp % 10) + s;
  } else if (tmp <= 61) {
    tmp -= 30;
    s = "07-" + std::to_string(tmp / 10) + std::to_string(tmp % 10) + s;
  } else if (tmp <= 92) {
    tmp -= 61;
    s = "08-" + std::to_string(tmp / 10) + std::to_string(tmp % 10) + s;
  } else {
    tmp -= 92;
    s = "09-" + std::to_string(tmp / 10) + std::to_string(tmp % 10) + s;
  }
  return s;
}


int date_time::operator-(const date_time &other) const {
  return time - other.time; /*
  int a, b;
  if (this->date_.day[1] == '6') {
    a = 0;
  } else if (this->date_.day[1] == '7') {
    a = 30;
  } else if (this->date_.day[1] == '8') {
    a = 61;
  } else {
    a = 92;
  }
  a += (this->date_.day[3] - '0') * 10 + (this->date_.day[4] - '0');
  a *= 24;
  a += (this->time_.hm[0] - '0') * 10 + (this->time_.hm[1] - '0');
  a *= 60;
  a += (this->time_.hm[3] - '0') * 10 + this->time_.hm[4] - '0';

  if (other.date_.day[1] == '6') {
    b = 0;
  } else if (other.date_.day[1] == '7') {
    b = 30;
  } else if (other.date_.day[1] == '8') {
    b = 61;
  } else {
    b = 92;
  }
  b += (other.date_.day[3] - '0') * 10 + (other.date_.day[4] - '0');
  b *= 24;
  b += (other.time_.hm[0] - '0') * 10 + (other.time_.hm[1] - '0');
  b *= 60;
  b += (other.time_.hm[3] - '0') * 10 + (other.time_.hm[4] - '0');
  return a - b;*/
}

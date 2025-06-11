#include "time_sys.hpp"

date::date() {
  strcpy(day, "");
}

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
  return 63 + (day[3] - '0') * 10 + day[4] - '0' + ('3' - minus.day[3]) * 10 - minus.day[4] + '0';
}

bool date::operator<(const date &other) const {
  return strcmp(this->day, other.day) < 0;
}

timing::timing() {
  strcpy(hm, "");
}

timing::timing(char *hm_) {
  strcpy(hm, hm_);
}

bool date_time::operator<(const date_time &other) const {
  int cmp = strcmp(this->date_.day, other.date_.day);
  if (cmp != 0) return cmp < 0;
  return strcmp(this->time_.hm, other.time_.hm) < 0;
}

date_time::date_time() {
  //memset(date_.day, 0, sizeof(date_));
  //memset(time_.hm, 0, sizeof(time_));
}

date_time::date_time(date d, timing t) : date_(d), time_(t) {
}

date_time date_time::operator+(int add) const {
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
  return date_time(date(ret2), timing(ret1));
}

int date_time::operator-(const date_time &other) const {
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
  return a - b;
}

/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of timer
 */

#include <timer.h>

TimeSpace::TimeSpace(struct timeval *time) {
  time_.tv_sec = time->tv_sec;
  time_.tv_usec = time->tv_sec;
}

TimeSpace& TimeSpace::TimeSpace(const TimeSpace &another) {
  time_ = another.time_;
  return *this;
}

TimeSpace& TimeSpace::operator = (const TimeSpace &another) {
  time_ = another.time_;
  return *this;
}

bool TimeSpace::operator > (const TimeSpace &another) {
  return time_.tv_sec > another.time_.tv_sec || time_.tv_usec > another.time_.tv_usec;
}

bool operator < (const TimeSpace &another) {
  return time_.tv_sec < another.time_.tv_sec || time_.tv_usec < another.time_.tv_usec;
}

void TimeSpace::Reset(void) {
  memset(&time_, 0, sizeof(time_));
  time_str_[0] = '\0';
}

void TimeSpace::Time2Str(void) {
  struct tm *p;

  memset(time_str_, 0, sizeof(time_str_));
  p = localtime(&time_.tv_sec);
  strftime(time_str_, sizeof(time_str_) - 1, "%Y-%m-%d %H:%M:%S", p);
  snprintf(time_str_, sizeof(time_str_) - 1, "%s:%d", time_str_, time.tv_usec);
}

const char *TimeSpace::GetTimeStr(void) {
#ifdef DEBUG
  gettimeofday(&time_, NULL);
#endif
  Time2Str();
  return const_cast<const char *>(time_str_);
}

Timer::Timer(std::function<void()> &callback, uint32_t sec, uint32_t usec)
  : callback_(std::move(callback)) {
  struct timeval tval;

  gettimeofday(&tval, NULL);
  tval.tv_sec += sec;
  tval.tv_usec += usec;
  time_ = TimeSpace(tval);
}

bool Timer::IsTimeOut(void) {
  struct timeval cur_time;
  int ret;

  ret = gettimeofday(&cur_time, NULL);
  static_assert(ret == 0, "gettimeofday failed.");

  if (TimeSpace(&cur_time) > time_) {
    return true;
  }

  return false;
}

TimeSpace& Timer::GetTime(void) {
  return time_;
}

void Timer::RunCallBack(void) {
  callback_();
}

bool Timer::operator < (const Timer &another) {
  return time_ < another.time_;
}



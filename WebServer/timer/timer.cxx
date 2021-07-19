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

void TimeSpace::Reset(void) {
  memset(&time_, 0, sizeof(time_));
}

Timer::Timer(TimeSpace &time) {
  time_ = time;
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


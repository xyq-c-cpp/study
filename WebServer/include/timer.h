/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of timer
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include <time.h>

class TimeSpace{
 public:
  TimeSpace() = default;
  TimeSpace(struct timeval *time);
  ~TimeSpace() = default;

  TimeSpace& TimeSpace(const TimeSpace &another);
  TimeSpace& operator = (const TimeSpace &another);

  bool operator > (const TimeSpace &another);
  void Reset(void);

 private:
  struct timeval time_;
};

class Timer {
 public:
  Timer(TimeSpace &time);
  ~Timer() = default;

  bool IsTimeOut(void);
  TimeSpace& GetTime(void);

 private:
  TimeSpace time_;
};

#endif /* _TIMER_H_ */


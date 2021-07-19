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
  bool operator < (const TimeSpace &another);
  void Reset(void);
  const char *GetTimeStr(void);

 private:
  void Time2Str(void);

  struct timeval time_;
  char time_str_[WEB_SVR_BUFF_SIZE_64];
};

class Timer {
 public:
  Timer(std::function<void()> &callback, uint32_t sec, uint32_t usec = 0);
  ~Timer() = default;
  bool operator < (const Timer &another);

  bool IsTimeOut(void);
  TimeSpace& GetTime(void);
  void RunCallBack(void);

 private:
  std::functional<void()> callback_;
  TimeSpace time_;
};

class TimerQueue {
 public:
  TimerQueue() = default;
  ~TimerQueue() = default;

 private:
  std::priority_queue<Timer, std::vector<Timer> > max_heap;
};

#endif /* _TIMER_H_ */


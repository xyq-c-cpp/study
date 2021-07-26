/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of timer
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include <memory>
#include <queue>
#include <sys/time.h>

#define WEB_SVR_BUFF_SIZE_64  64

class TimeSpace{
 public:
  TimeSpace() = default;
  TimeSpace(struct timeval *time);
  ~TimeSpace() = default;

  TimeSpace(const TimeSpace &another);
  TimeSpace& operator = (const TimeSpace &another);

  bool operator > (const TimeSpace &another) const;
  bool operator < (const TimeSpace &another) const;
  void Reset(void);
  struct timeval *GetTimePtr(void);
  const char *GetTimeStr(void);

 private:
  void Time2Str(void);

  struct timeval time_;
  char time_str_[WEB_SVR_BUFF_SIZE_64];
};

class Timer {
 public:
  Timer(std::function<void()> &callback, unsigned int sec, unsigned int usec = 0);
  ~Timer() = default;
  bool operator < (const Timer &another) const;

  bool IsTimeOut(void);
  TimeSpace& GetTime(void);
  void RunCallBack(void);

 private:
  std::function<void()> callback_;
  TimeSpace time_;
};

class TimerQueue {
 public:
  TimerQueue();
  ~TimerQueue() = default;

 private:
  std::priority_queue<Timer, std::vector<Timer> > time_queue_;
};

#endif /* _TIMER_H_ */


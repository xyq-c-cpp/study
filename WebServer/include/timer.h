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
#include <common.h>
#include <algorithm>

#define WEB_SVR_BUFF_SIZE_64  64

class TimeSpace {
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
  void SetTime(struct timeval *time);

 private:
  void Time2Str(void);

  struct timeval time_;
  char time_str_[WEB_SVR_BUFF_SIZE_64];
};

class Timer : public std::enable_shared_from_this<Timer> {
 public:
  Timer(TimerCb callback, unsigned int sec, unsigned int usec = 0,
    bool is_repeat = false);
  ~Timer() = default;
  bool operator < (const Timer &another) const;
  bool operator > (const Timer &another) const;

  bool IsTimeOut(void);
  TimeSpace& GetTime(void);
  int RunCallBack(void);
  void ComputeTimeAgain(void);
  bool IsRepeat(void) {
    return is_repeat_;
  }

 private:
  TimerCb callback_;
  const unsigned int sec_;
  const unsigned int usec_;
  const bool is_repeat_;
  TimeSpace time_;
};

/* <, big root heap */
struct cmp {
  bool operator()(const Timer *x, const Timer *y) {
    return *x < *y;
  }
};

template<typename T, typename compare>
class custom_priority_queue : public std::priority_queue<T, 
    std::vector<T>, compare> {
 public:
  bool remove(const T& value) {
    /* std::find just for base structure, not class 
     * std::find_if could be used to find the class.
     */
    auto it = find_if(this->c.begin(), this->c.end(), [value](T& n) {
      return value == n;
    });
    if (it != this->c.end()) {
      this->c.erase(it);
      std::make_heap(this->c.begin(), this->c.end(), this->comp);
      return true;
    } else {
      return false;
    }
  }
};

class TimerQueue {
 public:
  TimerQueue();
  ~TimerQueue() = default;
  Timer *AddTimer(TimerCb cb, int sec, int usec, bool is_repeat);
  bool DelTimer(Timer *timer);
  void TimerProc(void);
  int GetTimeOfLateTimeOut(void);

 private:
  custom_priority_queue<Timer *, cmp> timer_queue_;
};

#endif /* _TIMER_H_ */


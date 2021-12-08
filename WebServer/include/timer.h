#pragma once

#include <Common.h>
#include <chrono>

class Timer {
public:
  Timer(std::shared_ptr<Channal> channal, int msec);
  ~Timer();
  size_t getExpiredTime();
  bool isDeleted();
  bool isValid();
  void setDeleted();

private:
  std::shared_ptr<Channal> channal_;
  long long expiredTime_;
  bool isDeleted_;
};

//> 3 2 1 优先级队列根在数组或者队列后面，所以优先级队列栈顶 1
struct cmp {
  bool operator()(std::shared_ptr<Timer> &a, std::shared_ptr<Timer> &b) {
    return a->getExpiredTime() > b->getExpiredTime();
  }
};

class TimerManager {
public:
  TimerManager() = default;
  ~TimerManager() = default;
  void handleExpired();
  std::shared_ptr<Timer> addTimer(std::shared_ptr<Channal> channal, int msec);

private:
  std::priority_queue<std::shared_ptr<Timer>,
                      std::deque<std::shared_ptr<Timer>>, cmp>
      timerQueue_;
};
/*
 * author: 657023321@qq.com
 * date: 2021-10-06
 * function:
 */

#pragma once

#include <Common.h>

class EventLoopThreadPool {
public:
  explicit EventLoopThreadPool(int threadNum);
  ~EventLoopThreadPool() = default;
  std::shared_ptr<EventLoopThread> getNextLoopThread();
  void start();

private:
  const int threadNum_;
  int currentIndex_;
  std::vector<std::shared_ptr<EventLoopThread>> worker_;
};

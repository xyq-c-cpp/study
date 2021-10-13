/*
 * author: 657023321@qq.com
 * date: 2021-10-06
 * function: 
 */

#pragma once

#include <Common.h>

class EventLoopThread {
public:
  explicit EventLoopThread();
  ~EventLoopThread();
  void setThreadName(std::string name);
  void runInLoop(Task&& task);
  std::shared_ptr<EventLoop> getEventLoop();
  void start();

private:
  bool isInLoopThread();
  void wakeup();
  int handleWakeupEvent();

  std::shared_ptr<EventLoop> mainLoop_;
  std::thread thread_;
  int wakeupFd;
};

#pragma once

#include <Common.h>

class EventLoop {
public:
  EventLoop();
  ~EventLoop() = default;
  void queueInLoop(Task&& task);
  std::shared_ptr<Epoller> getEpoll();
  void loop();
  void stop();
  bool getIsHandlePending();

private:
  std::shared_ptr<Epoller> eventLoop_;
  bool isRuning_;
  bool isHandlePending_;
  std::queue<Task> pendingTask_;
  std::mutex lock_;
};

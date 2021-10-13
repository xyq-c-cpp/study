

#include <EventLoop.h>
#include <Epoller.h>
#include <Timer.h>
#include <sstream>

EventLoop::EventLoop()
  : eventLoop_(std::make_shared<Epoller>()), 
    isRuning_(true),
    isHandlePending_(false) {
  
}

std::shared_ptr<Epoller> EventLoop::getEpoll() {
  return eventLoop_;
}

void EventLoop::queueInLoop(Task&& task) {
  std::unique_lock<std::mutex> lock(lock_);
  pendingTask_.push(std::move(task));
}

void EventLoop::stop() {
  isRuning_ = false;
}

bool EventLoop::getIsHandlePending() {
  return isHandlePending_;
}

void EventLoop::loop() {
  std::stringbuf buf;
  std::ostream os(&buf);
  os << std::this_thread::get_id();

  while (isRuning_) {
    eventLoop_->epollWait();

    std::queue<Task> task;
    {
      std::unique_lock<std::mutex> lock(lock_);
      if (pendingTask_.empty())
        goto next;
      task.swap(pendingTask_);
    }
    isHandlePending_ = true;
    while (!task.empty()) {
        Task cb = std::move(task.front());
        task.pop();
        int ret;
        if (cb)
          ret = cb();
    }
    isHandlePending_ = false;
  next:
    eventLoop_->handleExpired();
  }
}

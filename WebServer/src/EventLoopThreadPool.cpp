/*
 * author: 657023321@qq.com
 * date: 2021-10-06
 * function:
 */

#include <EventLoopThread.h>
#include <EventLoopThreadPool.h>

EventLoopThreadPool::EventLoopThreadPool(int threadNum)
    : threadNum_(threadNum), currentIndex_(0) {
  worker_.resize(threadNum_);
  for (int i = 0; i < threadNum_; ++i) {
    worker_[i] = (std::make_shared<EventLoopThread>());
    worker_[i]->setThreadName((std::string("ELT-") + std::to_string(i)));
  }
}

std::shared_ptr<EventLoopThread> EventLoopThreadPool::getNextLoopThread() {
  int index = currentIndex_ % threadNum_;
  ++currentIndex_;
  return worker_[index];
}

void EventLoopThreadPool::start() {
  for (auto &i : worker_)
    i->start();
}

/*
 * author: 657023321@qq.com
 * date: 2021-10-06
 * function:
 */

#include <Channal.h>
#include <Epoller.h>
#include <EventLoop.h>
#include <EventLoopThread.h>
#include <Timer.h>
#include <sstream>

#ifndef _MSC_VER
#include <pthread.h>
#include <sys/eventfd.h>
#endif

EventLoopThread::EventLoopThread()
    : mainLoop_(std::make_shared<EventLoop>()),
      thread_(std::thread(std::bind(&EventLoop::loop, mainLoop_))),
      wakeupFd(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)) {
  assert(wakeupFd >= 0);
  std::stringbuf buf;
  std::ostream os(&buf);
  os << thread_.get_id();
  std::shared_ptr<Channal> wakeupChannal(
      std::make_shared<Channal>(wakeupFd, mainLoop_->getEpoll()));
  wakeupChannal->setEvent(EPOLLIN | EPOLLET);
  wakeupChannal->setIsUpdateEvent(false);
  wakeupChannal->setReadCb(
      std::bind(&EventLoopThread::handleWakeupEvent, this));
  bool ret = mainLoop_->getEpoll()->epollAdd(wakeupChannal);
  assert(ret == true);
}

EventLoopThread::~EventLoopThread() {
  mainLoop_->stop();
  thread_.join();
}

void EventLoopThread::runInLoop(Task &&task) {
  if (isInLoopThread()) {
    task();
  } else {
    mainLoop_->queueInLoop(std::move(task));
    if (!mainLoop_->getIsHandlePending())
      wakeup();
  }
}

inline bool EventLoopThread::isInLoopThread() {
  return thread_.get_id() == std::this_thread::get_id();
}

void EventLoopThread::wakeup() {
  uint64_t tt = 1;
  (void)web_svr_write(wakeupFd, reinterpret_cast<char *>(&tt), sizeof(tt));
}

int EventLoopThread::handleWakeupEvent() {
  uint64_t tt = 0;
  (void)web_svr_read(wakeupFd, reinterpret_cast<char *>(&tt), sizeof(tt));
#ifdef DEBUG
  logger() << "handle wakeup event, fd " << wakeupFd << " tt " << tt;
#endif
  return 0;
}

void EventLoopThread::start() {}

std::shared_ptr<EventLoop> EventLoopThread::getEventLoop() { return mainLoop_; }

void EventLoopThread::setThreadName(std::string name) {
  //线程名字需要小于16个字节
  int ret = pthread_setname_np(thread_.native_handle(), name.c_str());
  assert(ret == 0);
}

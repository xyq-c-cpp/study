/*
 * author: 657023321@qq.com
 * date: 2021-07-18
 * function: the class of epoller, wraping epoll
 */

#include <Channal.h>
#include <Epoller.h>
#include <Server.h>
#include <Timer.h>

Epoller::Epoller(int maxFdNum)
    : maxFdNum_(maxFdNum), fd_(epoll_create(maxFdNum_)),
      eventArray_(new struct epoll_event[maxFdNum_]),
      timerManager_(std::make_shared<TimerManager>()) {
  assert(fd_ >= 0);
  assert(eventArray_.get() != nullptr);
  assert(timerManager_ != nullptr);
}

Epoller::~Epoller() {
#ifdef DEBUG
  logger() << "~epoller fd " << fd_;
#endif
  (void)close(fd_);
}

bool Epoller::epollAdd(std::shared_ptr<Channal> channal, int msec) {
  auto i = fd2channal_.find(channal->getFd());
  if (i != fd2channal_.end()) {
#ifdef DEBUG
    logger() << "tid " << std::this_thread::get_id()
             << " epoll_add failed, has fd " << channal->getFd() << " exist";
#endif
    return false;
  }
  if (msec > 0) {
    auto timer = timerManager_->addTimer(channal, msec);
    if (timer == nullptr) {
#ifdef DEBUG
      logger() << "epoll_add add timer failed, fd " << channal->getFd();
#endif
      return false;
    }
    channal->setTimer(timer);
  }

  if (epollCtl(EPOLL_CTL_ADD, channal->getFd(), channal->getEvent())) {
    auto ret = fd2channal_.insert(std::make_pair(channal->getFd(), channal));
    return ret.second;
  }
#ifdef DEBUG
  logger() << "tid " << std::this_thread::get_id() << "epoll_add failed, fd "
           << channal->getFd();
#endif
  channal->delTimer();
  return false;
}

bool Epoller::epollMod(int fd, uint32_t event, int msec) {
  auto i = fd2channal_.find(fd);
  if (i == fd2channal_.end()) {
#ifdef DEBUG
    logger() << "tid " << std::this_thread::get_id()
             << "epoll_mod failed, not find fd " << fd;
#endif
    return false;
  }
  if (msec > 0) {
    auto timer = timerManager_->addTimer(fd2channal_[fd], msec);
    if (timer == nullptr) {
#ifdef DEBUG
      logger() << "epoll_mod add timer failed, fd " << fd;
#endif
      return false;
    }
    fd2channal_[fd]->setTimer(timer);
  }

  bool ret = epollCtl(EPOLL_CTL_MOD, fd, event);
  if (!ret) {
    fd2channal_[fd]->delTimer();
    return false;
  }
  return true;
}

bool Epoller::epollDel(int fd) {
  auto i = fd2channal_.find(fd);
  if (i == fd2channal_.end()) {
#ifdef DEBUG
    logger() << "tid " << std::this_thread::get_id()
             << "epoll_del, not find fd " << fd;
#endif
    return false;
  }

  bool ret = epollCtl(EPOLL_CTL_DEL, fd, 0);
  if (!ret) {
#ifdef DEBUG
    logger() << "tid " << std::this_thread::get_id() << "epoll_del failed, fd "
             << fd;
#endif
    fd2channal_.erase(fd);
    return false;
  }
  fd2channal_.erase(fd);
  return true;
}

inline bool Epoller::epollCtl(int op, int fd, uint32_t event) {
  struct epoll_event myEvent;

  memset(&myEvent, 0, sizeof(myEvent));
  myEvent.data.fd = fd;
  myEvent.events = event;
  int ret = epoll_ctl(fd_, op, fd, &myEvent);

  return ret == 0 ? true : false;
}

void Epoller::handleExpired() { timerManager_->handleExpired(); }

void Epoller::epollWait(int msec) {
  int ret;
#define DEFAULT_WAIT_TIME 10 * 1000

  struct epoll_event *ptr = eventArray_.get();
  ret = epoll_wait(fd_, ptr, maxFdNum_, DEFAULT_WAIT_TIME);
  if (ret <= 0) {
    //#ifdef DEBUG
    //    logger()
    //        << "wake up from epoll_wait, but not events, to handle
    //        timerExpired";
    //#endif
    return;
  }

#ifdef DEBUG
  logger() << "tid " << std::this_thread::get_id() << " epoll_wait get " << ret
           << " events to handle";
#endif

  int eventFd, localEvent;
  for (int i = 0; i < ret; ++i) {
#ifdef DEBUG
    logger() << "tid " << std::this_thread::get_id() << " epoll_wait fd "
             << ptr[i].data.fd << " events " << ptr[i].events;
#endif
    std::shared_ptr<Channal> &channal = fd2channal_[ptr[i].data.fd];
    if (channal == nullptr) {
#ifdef DEBUG
      logger() << "tid " << std::this_thread::get_id()
               << " epoll_wait channal == nullptr";
#endif
      continue;
    }
    channal->updateEventAndLastEvent(ptr[i].events);
#ifdef DEBUG
    ret = channal->handleEvent();
    logger() << "tid " << std::this_thread::get_id() << " handleEvent RET "
             << ret;
#else
    (void)channal->handleEvent();
#endif
  }
}

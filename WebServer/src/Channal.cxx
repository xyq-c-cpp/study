/*
 * author: 657023321@qq.com
 * date: 2021-07-18
 * function: the class of socket channal entity, contain the action of
 * read-write events
 */

#include <Channal.h>
#include <Epoller.h>
#include <Message.h>
#include <Server.h>
#include <Timer.h>
#include <iostream>

#define MAX_RETRY_TIME 3

Channal::Channal(int fd, std::shared_ptr<Epoller> epoller)
    : fd_(fd), epoller_(epoller), isErase_(false), isUpdate_(false) {
#ifdef DEBUG
  std::cout << "channal init, fd " << fd_ << std::endl;
#endif
}

Channal::~Channal() {
#ifdef DEBUG
  std::cout << "~channal, fd " << fd_ << std::endl;
#endif
  (void)close(fd_);
}

void Channal::setWriteCb(EventCb &&cb) { writeCb_ = std::move(cb); }

void Channal::setReadCb(EventCb &&cb) { readCb_ = std::move(cb); }

void Channal::setErrorCb(EventCb &&cb) { errorCb_ = std::move(cb); }

void Channal::setEvent(uint32_t event) { event_ = event; }

void Channal::setMsg(std::shared_ptr<Message> msg) { msg_ = msg; }

void Channal::setIsUpdateEvent(bool isUpdate) { isUpdate_ = isUpdate; }

int Channal::handleEvent() {
  uint32_t localEvent = event_;

  if ((localEvent & EPOLLHUP) && !(localEvent & EPOLLIN)) {
#ifdef DEBUG
    std::cout << "EPOLLHUP fd " << fd_ << std::endl;
#endif
    isErase_ = true;
  } else if (localEvent & EPOLLERR) {
    if (errorCb_)
      (void)errorCb_();
  } else if (localEvent & (EPOLLIN | EPOLLPRI | EPOLLHUP)) {
    if (readCb_)
      (void)readCb_();
  } else if (localEvent & EPOLLOUT) {
    if (writeCb_)
      (void)writeCb_();
  }

  delTimer();
  auto epoller = epoller_.lock();
  if (epoller != nullptr) {
    if (isErase_)
      return epoller->epollDel(fd_);
    else if (isUpdate_) // KEEP-ALIVE
      return epoller->epollMod(fd_, event_, KEEP_ALIVE_TIMEOUT_MS);
    else {
#ifdef DEBUG
      std::cout << "do nothing, and not timer, lastevent " << lastEvent_
                << std::endl;
#endif
      return 0;
    }
  } else {
#ifdef DEBUG
    std::cout << "epoller weak_ptr to shared_ptr failed" << std::endl;
#endif
    return -1;
  }
}

int &Channal::getFd() { return fd_; }

uint32_t &Channal::getEvent() { return event_; }

void Channal::updateEventAndLastEvent(uint32_t evnet) {
  lastEvent_ = event_;
  event_ = evnet;
}

void Channal::setErase(bool isErase) { isErase_ = isErase; }

void Channal::setTimer(std::shared_ptr<Timer> timer) { timer_ = timer; }

int Channal::handleClose() {
  auto epoller = epoller_.lock();
  if (epoller == nullptr) {
    return -1;
  }
  bool ret = epoller->epollDel(fd_);
  return ret;
}

void Channal::delTimer() {
  if (timer_.expired())
    return;
  std::shared_ptr<Timer> t = timer_.lock();
  if (t == nullptr)
    return;
  t->setDeleted();
  t.reset();
  timer_.reset();
}
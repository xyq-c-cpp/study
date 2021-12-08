/*
 * author: 657023321@qq.com
 * date: 2021-07-18
 * function: the class of socket channal entity, contain the action of
 * read-write events
 */

#ifndef _CHANNAL_H_
#define _CHANNAL_H_

#include <Common.h>

class Channal {
public:
  explicit Channal(int fd, std::shared_ptr<Epoller> epoller);
  ~Channal();
  int handleEvent();
  void setWriteCb(EventCb &&cb);
  void setReadCb(EventCb &&cb);
  void setErrorCb(EventCb &&cb);
  void setEvent(uint32_t event);
  void setMsg(std::shared_ptr<Message> msg);
  void setErase(bool isErase);
  void setIsUpdateEvent(bool isUpdate);
  void updateEventAndLastEvent(uint32_t event);
  int &getFd();
  uint32_t &getEvent();
  void setTimer(std::shared_ptr<Timer> timer);
  void delTimer();
  int handleClose();

private:
  int fd_;
  std::weak_ptr<Epoller> epoller_;
  bool isErase_;
  uint32_t event_;
  bool isUpdate_;
  uint32_t lastEvent_;
  std::weak_ptr<Timer> timer_;
  EventCb writeCb_;
  EventCb readCb_;
  EventCb errorCb_;
  std::shared_ptr<Message> msg_;
};

#endif

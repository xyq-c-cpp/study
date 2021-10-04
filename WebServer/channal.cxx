/*
 * author: 657023321@qq.com
 * date: 2021-07-18
 * function: the class of socket channal entity, contain the action of read-write 
 *           events
 */

#include <channal.h>
#include <server.h>
#include <iostream>
#include <message.h>
#include <epoller.h>

#define MAX_RETRY_TIME      3

Channal::Channal(int fd, Server *server)
  : fd_(fd),
    server_(server),
    in_pos_(0),
    out_pos_(0),
    retry_time_(0) {
  LOG_DEBUG("init channal, fd %d", fd);
}

Channal::~Channal() {
  int ret;
  do {
    ret = close(fd_);
    LOG_DEBUG("channal destroy, close fd %d, ret %d errno %d %s",
      fd_, ret, ret ? errno : 0, strerror(ret == 0 ? 0 : errno));
  } while (ret != 0);
}

std::shared_ptr<Channal> Channal::GetSharedPtrFromThis(void) {
  return shared_from_this();
}

void Channal::AddReadEvent(Epoller *epoller) {
  epoller->AddReadEvent(fd_, std::bind(&Channal::ReadEventProc, 
    GetSharedPtrFromThis(), epoller), EPOLLIN | EPOLLET | EPOLLONESHOT);
}

void Channal::AddWriteEvent(Epoller *epoller) {
  epoller->AddWriteEvent(fd_, std::bind(&Channal::WriteEventProc, 
    GetSharedPtrFromThis(), epoller), EPOLLOUT | EPOLLET | EPOLLONESHOT);
}

int Channal::ReadEventProc(Epoller *epoller) {
  int read_nr;

  memset(in_buffer_, 0, sizeof(in_buffer_));
  read_nr = web_svr_read(fd_, static_cast<char*>(in_buffer_), 
    sizeof(in_buffer_));
  if (read_nr <= 0) {
    LOG_ERROR("invalid read event, fd %d, read_nr %d, errno %d", fd_, 
      read_nr, errno);
    ++retry_time_;
    if (retry_time_ >= MAX_RETRY_TIME) {
        epoller->DelFd(fd_);
        return -1;
    }
    AddReadEvent(epoller);
    return -1;
  }

  LOG_DEBUG("read message, size %d, fd %d", read_nr, fd_);

  std::shared_ptr<Message> msg(new Message(std::string(in_buffer_)));
  if (!msg.get()) {
    LOG_ERROR("new message failed, channal fd %d", fd_);
    ++retry_time_;
    if (retry_time_ >= MAX_RETRY_TIME) {
        epoller->DelFd(fd_);
        return -1;
    }
    AddReadEvent(epoller);
    return -1;
  }

  server_->TaskInQueue(std::bind(&Message::ProcMessage, msg, 
    GetSharedPtrFromThis()));

  return 0;
}

Server *Channal::GetServer() {
    return server_;
}

int Channal::WriteEventProc(Epoller *epoller) {
  int ret;

  {
    std::lock_guard<std::mutex> lock(lock_);

    if (out_pos_ == 0) {
      LOG_DEBUG("still not respone, add write event again, fd %d", fd_);
      AddWriteEvent(epoller);
      return 0;
    }
 
    ret = web_svr_write(fd_, out_buffer_, out_pos_);
    if (ret != static_cast<int>(out_pos_)) {
      LOG_WARN("write num less than expected num, ret %d, expected %d", ret, 
        out_pos_);
    }

    LOG_DEBUG("write fd bytes stream, fd %d, write num %d, errno %d", fd_, ret, 
      errno);
    out_pos_ = 0;
  }

  AddReadEvent(epoller);

  return 0;
}

int Channal::WriteRsp(const char *buff, int len) {
#if 0
  if (out_pos_ + len > sizeof(out_buffer_)) {
    LOG_ERROR("out buffer is full, abort it");
    return -1;
  }

  {
    std::lock_guard<std::mutex> lock(lock_);
    memcpy(&out_buffer_[out_pos_], buff, len);
    out_pos_ += len;
  }
#endif
    int ret = web_svr_write(fd_, const_cast<char *>(buff), len);
    if (ret != len) {
      LOG_ERROR("write num less than expected num, ret %d, expected %d, errno %d",
        ret, len, errno);
      return -1;
    }

  return 0;
}

int Channal::Fd(void) {
  return fd_;
}


/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of socket channal entity, contain the action of read-write 
 *           events
 */

#include <channal.h>
#include <server.h>
#include <iostream>
#include <message.h>
#include <epoller.h>

Channal::Channal(int fd, Server *server)
  : fd_(fd),
    server_(server),
    in_pos_(0),
    out_pos_(0) {
  LOG_DEBUG("init channal, fd %d", fd);
}

Channal::~Channal() {
  LOG_DEBUG("close fd %d", fd_);
  close(fd_);
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
  if (read_nr < 0) {
    LOG_ERROR("invalid read event, fd %d, read_nr %d, errno %d", fd_, 
      read_nr, errno);
    AddReadEvent(epoller);
    return -1;
  }
  LOG_DEBUG("read msg, size %d, fd %d", read_nr, fd_);

  std::shared_ptr<Message> msg(new Message(std::string(in_buffer_)));
  if (!msg.get()) {
    LOG_ERROR("new message failed, channal fd %d", fd_);
    AddReadEvent(epoller);
    return -1;
  }

  server_->TaskInQueue(std::bind(&Message::ProcMessage, msg, 
    GetSharedPtrFromThis()));
  AddWriteEvent(epoller);
  return 0;
}

int Channal::WriteEventProc(Epoller *epoller) {
  {
    std::lock_guard<std::mutex> lock(lock_);

    LOG_DEBUG("out_pos %d", out_pos_);
    if (out_pos_ == 0) {
      LOG_DEBUG("still not respone, add write event again, fd %d", fd_);
      AddWriteEvent(epoller);
      return 0;
    }
    int ret = web_svr_write(fd_, out_buffer_, out_pos_);
    if (ret != out_pos_) {
      LOG_WARN("write num less than expected num, ret %d, expected %d", ret, 
        out_pos_);
    }
    LOG_DEBUG("write fd %d bytes stream, ret %d, errno %d", fd_, ret, errno);
    out_pos_ = 0;
  }
  AddReadEvent(epoller);

  return 0;
}

int Channal::WriteRsp(const char *buff, int len) {
  if (out_pos_ + len > sizeof(out_buffer_)) {
    LOG_ERROR("out buffer is full, abort it");
    return -1;
  }

  {
    std::lock_guard<std::mutex> lock(lock_);
    memcpy(&out_buffer_[out_pos_], buff, len);
    out_pos_ += len;
  }
  
  return 0;
}

int Channal::Fd(void) {
  return fd_;
}


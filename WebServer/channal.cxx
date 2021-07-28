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

Channal::Channal(int fd, Server *server)
  : fd_(fd),
    server_(server),
    in_pos_(0),
    out_pos_(0) {

}

Channal::~Channal() {
  close(fd_);
}

std::shared_ptr<Channal> Channal::GetSharedPtrFromThis(void) {
  return shared_from_this();
}

int Channal::ReadEventProc(Epoller *epoller) {
  int read_nr;

  read_nr = web_svr_read(fd_, static_cast<char*>(in_buffer_), 
    sizeof(in_buffer_));
  if (read_nr <= 0) {
    LOG_ERROR("invalid read event, fd %d", fd_);
    return -1;
  }

  std::shared_ptr<Message> msg(new Message(std::string(in_buffer_, read_nr)));
  if (!msg.get()) {
    LOG_ERROR("new message failed, channal fd %d", fd_);
    return -1;
  }

  server_->TaskInQueue(std::bind(&Message::ProcMessage, msg, 
    GetSharedPtrFromThis()));
}

int Channal::WriteEventProc(Epoller *epoller) {
  {
    std::lock_guard<std::mutex> lock(lock_);
    int ret = web_svr_write(fd_, out_buffer_, out_pos_);
    if (ret != out_pos_) {
      LOG_WARN("write num less than expected num, ret %d, expected %d", ret, 
        out_pos_);
    }
    out_pos_ = 0;
  }

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


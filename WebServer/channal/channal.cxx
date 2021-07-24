/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of socket channal entity, contain the action of read-write 
 *           events
 */

#include <channal.h>

Channal::Channal(int fd)
  : fd_(fd) {

}

Channal::~Channal() {
  close(fd_);
}

int Channal::ReadEventProc(void) {
  int read_nr;
  Message *msg;

  read_nr = web_svr_read(fd_, static_cast<uint8_t *>(in_buffer_), 
    sizeof(in_buffer_));
  if (read_nr <= 0) {
    LOG_ERROR("invalid read event, fd %d", fd_);
    return -1;
  }

  msg = new Message(fd_, std::string(in_buffer_, read_nr));
  
}

int Channal::WriteEventProc(void) {

}

int Channal::Fd(void) {
  return const_cast<int>(fd_);
}


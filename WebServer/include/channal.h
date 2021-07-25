/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of socket channal entity, contain the action of read-write 
 *           events
 */

#ifndef _CHANNAL_H_
#define _CHANNAL_H_

#include <common.h>

#define CHANNAL_IN_BUFF_SIZE    8192
#define CHANNAL_OUT_BUFF_SIZE   8192

class Channal : public std::enable_shared_from_this<Channal> {
 public:
  Channal(int fd, Server *server);
  ~Channal();
  int ReadEventProc(Epoller *epoller);
  int WriteEventProc(Epoller *epoller);
  int WriteRsp(const char *buff, int len);
  int Fd(void);
  std::shared_ptr<Channal> GetSharedPtrFromThis(void);

 private:
  const int fd_;
  Server *server_;
  int in_pos_, out_pos_;
  char in_buffer_[CHANNAL_IN_BUFF_SIZE];
  char out_buffer_[CHANNAL_OUT_BUFF_SIZE];

  std::mutex lock_;
};

#endif


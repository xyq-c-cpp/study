/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of socket channal entity, contain the action of read-write 
 *           events
 */

#ifndef _CHANNAL_H_
#define _CHANNAL_H_

#define CHANNAL_IN_BUFF_SIZE    8192
#define CHANNAL_OUT_BUFF_SIZE   8192

class Channal {
 public:
  Channal(int fd);
  ~Channal();
  int ReadEventProc();
  int WriteEventProc();
  int WriteRsp(const char *buff, int len);
  int Fd(void);

 private:
  const int fd_;
  char *in_buffer_[CHANNAL_IN_BUFF_SIZE];
  char *out_buffer_[CHANNAL_OUT_BUFF_SIZE];
};

#endif


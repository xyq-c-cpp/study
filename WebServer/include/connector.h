/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of action entity of socket connecting
 */

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <common.h>

class Connector : public std::enable_shared_from_this<Connector> {
 public:
  static Connector *CreateConnector(int port, Epoller *epoller, 
    int listen_cnt);
  int Connect(Epoller *epoller);
  ~Connector();
  void SetOwn(Server *server) {
    server_ = server; 
    assert(server_ != nullptr);
  }
  int Fd(void) const {return fd_;}
  
 private:
  Connector(int port, Epoller *epoller, int listen_cnt);

  const int port_;
  const int listen_cnt_;
  Server *server_;
  Epoller *epoller_;
  int fd_;
};

#endif /* _CONNECTOR_H_ */


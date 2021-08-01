/*
 * author: 657023321@qq.com
 * date: 2021-07-18
 * function: the class of action entity of socket connecting
 */

#include <connector.h>
#include <epoller.h>
#include <server.h>
#include <channal.h>

Connector *Connector::CreateConnector(int port,Epoller *epoller,
    int listen_cnt) {
  static Connector *tmp = nullptr;

  assert(port > 0 && port < 65536);
  assert(epoller != nullptr);

  if (!tmp) {
    tmp = new Connector(port, epoller, listen_cnt);
    assert(tmp != nullptr);
  }

  return tmp;
}

int Connector::Connect(Epoller *epoller) {
  int fd;
  struct sockaddr_in addr;
  int len;
  bool boolret;
  int ret;

  memset(&addr, 0, sizeof(addr));
  for (; ;) {
    fd = accept(fd_, reinterpret_cast<struct sockaddr *>(&addr), 
      reinterpret_cast<socklen_t *>(&len));
    if (fd <= 0) {
      LOG_ERROR("accept fd failed, fd val %d, errno %d, exit accept", fd, errno);
      break;
    }

    LOG_DEBUG("accept success, fd %d, addr %s", fd, inet_ntoa(addr.sin_addr));

    boolret = web_svr_set_fd_no_block(fd);
    if (!boolret) {
      LOG_ERROR("set fd %d no block failed, close it", fd);
      close(fd);
      continue;
    }

    std::shared_ptr<Channal> tmp(new Channal(fd, server_));
    if (!tmp.get()) {
      LOG_ERROR("new channal failed, fd %d", fd);
      close(fd);
      continue;
    }

    ret = epoller_->AddReadEvent(fd, std::bind(&Channal::ReadEventProc, 
      tmp->GetSharedPtrFromThis(), epoller), EPOLLIN | EPOLLONESHOT | EPOLLET);
    if (ret) {
      LOG_ERROR("add fd %d to epoll failed", fd);
      epoller_->DelFd(fd);
      continue;
    }

    server_->InsertChannal(std::make_pair(fd, tmp));

    LOG_DEBUG("create channal success, fd %d", fd);
  }

  LOG_DEBUG("finish connectting, exit from loop");

  return 0;
}

Connector::Connector(int port, Epoller *epoller, int listen_cnt)
  : port_(port),
    listen_cnt_(listen_cnt),
    server_(nullptr),
    epoller_(epoller),
    fd_(socket(AF_INET, SOCK_STREAM, 0)) {
  int ret;
  int reuseaddr = 1;
  struct sockaddr_in addr;

  assert(fd_ >= 0);
  assert(epoller_ != nullptr);

  ret = web_svr_set_fd_no_block(fd_);
  assert(ret == true);

  ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(&reuseaddr), 
    sizeof(reuseaddr));
  LOG_DEBUG("fd %d setsockopt ret %d, errno %d", fd_, ret, errno);
  assert(ret == 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(static_cast<unsigned int>(port));
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  ret = bind(fd_, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
  LOG_DEBUG("fd %d bind ret %d, errno %d", fd_, ret, errno);
  assert(ret == 0);

  ret = listen(fd_, listen_cnt_);
  LOG_DEBUG("fd %d listen ret %d, errno %d", fd_, ret, errno);
  assert(ret == 0);
}

Connector::~Connector() {
  LOG_DEBUG("close listen fd %d", fd_);
  close(fd_);
}


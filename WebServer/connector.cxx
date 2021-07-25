/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of action entity of socket connecting
 */

#include <connector.h>
#include <epoller.h>
#include <server.h>
#include <channal.h>

Connector *Connector::CreateConnector(int port, Server *server, 
    Epoller *epoller, int listen_cnt) {
  static Connector *tmp = nullptr;
  assert(port > 0 && port < 65536);
  assert(server != nullptr);

  if (!tmp) {
    tmp = new Connector(port, server, epoller, listen_cnt);
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
  int keepalive = 1;

  memset(&addr, 0, sizeof(addr));
  for (; ;) {
    fd = accept(fd_, reinterpret_cast<struct sockaddr *>(&addr), 
      reinterpret_cast<socklen_t *>(&len));
    if (fd <= 0) {
      LOG_ERROR("accept fd failed, fd val %d, exit accept", fd);
      break;
    }

    LOG_DEBUG("accept success, fd %d, addr %s", fd, inet_ntoa(addr.sin_addr));

    boolret = web_svr_set_fd_no_block(fd);
    if (!boolret) {
      LOG_ERROR("set fd %d no block failed, close it", fd);
      close(fd);
      continue;
    }

    ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, static_cast<void *>(&keepalive), 
      sizeof(keepalive));
    if (!ret) {
      LOG_ERROR("set fd %d option keepalive failed", fd);
      close(fd);
      continue;
    }

    std::shared_ptr<Channal> tmp(new Channal(fd, server_));
    if (!tmp.get()) {
      LOG_ERROR("new channal failed, fd %d", fd);
      close(fd);
      continue;
    }

    ret = epoller_->AddReadWriteEvent(fd, std::move(std::bind(&Channal::ReadEventProc, 
      tmp.get(), std::placeholders::_1)), std::move(std::bind(&Channal::WriteEventProc,
      tmp.get(), std::placeholders::_1)));
    if (ret) {
      LOG_ERROR("add fd %d to epoll failed", fd);
      epoller_->DelFd(fd);
      continue;
    }
    server_->Insert(std::make_pair(fd, tmp));

    LOG_DEBUG("create channal success, fd %d", fd);
  }
}

Connector::Connector(int port, Server *server, Epoller *epoller, int listen_cnt)
  : port_(port),
    listen_cnt_(listen_cnt),
    server_(server),
    epoller_(epoller),
    fd_(socket(AF_INET, SOCK_STREAM, 0)) {
  int ret;
  int reuseaddr = 1;
  struct sockaddr_in addr;
  unsigned int event = EPOLLIN | EPOLLET;

  assert(fd_ >= 0);
  assert(server_ != nullptr);
  assert(epoller_ != nullptr);

  ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(&reuseaddr), 
    sizeof(reuseaddr));
  assert(!ret);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(static_cast<unsigned int>(port));
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  ret = bind(fd_, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
  assert(!ret);

  ret = listen(fd_, listen_cnt_);
  assert(!ret);

  ret = web_svr_set_fd_no_block(fd_);
  assert(ret);

  ret = epoller_->AddReadEvent(fd_, std::move(std::bind(&Connector::Connect, *this,
    std::placeholders::_1)));
  assert(ret == 0);
}



/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of action entity of socket connecting
 */

#include <connector.h>

Connector *Connector::CreateConnector(int port, Server *server) {
  static Connector *tmp = nullptr;
  static_assert(port > 0 && port < 65536, "connect fd port should greater than 0"
    " and less than 65536");
  statoc_assert(server != nullptr, "server ptr is nullptr");

  if (!tmp) {
    tmp = new Connector(port, server);
    static_assert(tmp != nullptr, "Connector::CreateConnector new Connecter failed");
  }

  return tmp;
}

void Connector::Connect(void) {
  int fd;
  struct sockaddr_in addr;
  int len;
  bool boolret;
  int ret;
  int keepalive = 1;
  Channal *tmp = nullptr;

  memset(&addr, 0, sizeof(addr));
  for (; ;) {
    fd = accept(fd_, static_cast<struct sockaddr *>(&addr), &len);
    if (fd <= 0) {
      LOG_ERROR("accept fd failed, fd val %d, exit accept", fd);
      break;
    }

    LOG_DEBUG("accept success, fd %d, addr %s", fd, inet_ntoa(add.sin_addr));

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

    tmp = new Channal(fd);
    if (!tmp) {
      LOG_ERROR("new channal failed, fd %d", fd);
      close(fd);
      continue;
    }

    ret = epoller_.AddReadWriteEvent(fd, std::bind(&Channal::ReadEventProc(), *tmp),
      std::bind(&Channal::WriteEventProc(), *tmp));
    if (ret) {
      LOG_ERROR("add fd %d to epoll failed", fd);
      epoller_.DelFd(fd);
      delete tmp;
      continue;
    }
    server_.Insert(std::make_pair(fd, tmp));

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
  uint32_t event = EPOLLIN | EPOLLET;

  static_assert(fd_ >= 0, "get socket fd failed");
  static_assert(server_ != nullptr, "server_ ptr is nullptr");
  static_assert(epoller_ != nullptr, "epoller_ ptr is nullptr");

  ret = setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(&reuseaddr), 
    sizeof(reuseaddr);
  static_assert(!ret, "setsockopt failed");

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(static_cast<unsigned int>(port));
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  ret = bind(fd_, static_cast<struct sockaddr *>(&addr), sizeof(addr));
  static_assert(!ret, "bind failed");

  ret = listen(fd_, listen_cnt_);
  static_assert(!ret, "listen failed");

  ret = web_svr_set_fd_no_block(fd_);
  static_assert(ret, "set fd no block failed");

  ret = epoller_.AddReadEvent(fd_, std::bind(&Connector::Connect(), *this));
  static_assert(ret == 0, "add listen fd to epoll failed");
}



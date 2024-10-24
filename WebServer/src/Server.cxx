/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of web server class, a single class.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <Channal.h>
#include <Epoller.h>
#include <EventLoop.h>
#include <EventLoopThread.h>
#include <EventLoopThreadPool.h>
#include <Message.h>
#include <Server.h>
#include <Timer.h>

Server::Server(int port, std::shared_ptr<EventLoop> mainLoop, int threadNum)
    : port_(port), mainLoop_(mainLoop),
      pool_(std::make_shared<EventLoopThreadPool>(threadNum)) {
  acceptor_ = std::make_shared<Channal>(newListenFd(), mainLoop_->getEpoll());
  acceptor_->setEvent(EPOLLIN | EPOLLET);
  acceptor_->setReadCb(std::bind(&Server::acceptConnection, this));
  acceptor_->setIsUpdateEvent(false);
  maxOpenFile_ = getMaxOpenFileNum();
}

void Server::start() {
  bool ret = mainLoop_->getEpoll()->epollAdd(acceptor_);
  assert(ret == true);
  pool_->start();
}

int Server::newListenFd() {
  int ret;
  int reuseaddr = 1;
  struct sockaddr_in addr;
  const int maxListenNum = 65536;

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  assert((fd >= 0));

  ret = web_svr_set_fd_no_block(fd);
  assert(ret == true);

  ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                   static_cast<void *>(&reuseaddr), sizeof(reuseaddr));
  assert(ret == 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(static_cast<unsigned int>(port_));
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  ret = bind(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
  assert(ret == 0);

  ret = listen(fd, maxListenNum);
  assert(ret == 0);

  return fd;
}

int Server::acceptConnection() {
  int fd;
  struct sockaddr_in addr;
  int len = 1;
  bool ret;

  memset(&addr, 0, sizeof(addr));
  for (;;) {
    fd = accept(acceptor_->getFd(), reinterpret_cast<struct sockaddr *>(&addr),
                reinterpret_cast<socklen_t *>(&len));
    if (fd <= 0) {
      break;
    }
#ifdef DEBUG
    logger() << "accept success, fd " << fd << ", addr "
             << inet_ntoa(addr.sin_addr);
#endif
    //���д��� ���õ�������ϵͳ���û�ȡ��ǰ���̿ɴ򿪵��ļ�������������
    if (fd >= maxOpenFile_) {
#ifdef DEBUG
      logger() << "waring!!! the num of fd has been the most max";
#endif
      close(fd);
      continue;
    }

    ret = web_svr_set_fd_no_block(fd);
    if (!ret) {
      close(fd);
      continue;
    }

    // setSocketNoLinger(fd);
    setSocketNodelay(fd);

    auto loop = pool_->getNextLoopThread();
    std::shared_ptr<Channal> tmp(
        std::make_shared<Channal>(fd, loop->getEventLoop()->getEpoll()));
    std::shared_ptr<Message> msg(std::make_shared<Message>(tmp));
    tmp->setMsg(msg);
    tmp->setEvent(EPOLLIN | EPOLLET | EPOLLONESHOT);
    tmp->setReadCb(std::bind(&Message::handleReadEvent, msg));
    tmp->setWriteCb(std::bind(&Message::handleWriteEvent, msg));
    tmp->setErrorCb(std::bind(&Message::handleErrorEvnet, msg));
    tmp->setConnectHandleCb(std::bind(&Message::HandleConnectEvent, msg));
    loop->runInLoop(std::bind([tmp, loop]() -> int {
      bool ret =
          loop->getEventLoop()->getEpoll()->epollAdd(tmp, DEFAULT_TIMEOUT_MS);
      if (!ret) {
#ifdef DEBUG
        logger() << "add to epoll failed, fd " << tmp->getFd();
#endif
      }
      return ret;
    }));
#ifdef DEBUG
    logger() << "create channal success, fd " << fd;
#endif
  }
  return 0;
}

int Server::handleError() { return 0; }
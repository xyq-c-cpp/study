/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of web server class, a single class.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <thread>

#include <Server.h>
#include <filetype.h>
#include <common.h>
#include <requestmsg.h>

Server *CreateServer(uint32_t port, int thread_nr, int listen_cnt) {
  static Server *tmp = nullptr;
  if (!tmp) {
    tmp = new Server(port, thread_nr, listen_cnt);
    static_cast(tmp != nullptr, "new Server failed");
  }

  return tmp;
}

Server::Server(int port, int thread_nr, int listen_cnt)
  : port_(port),
    epoller_(Epoller::CreateEpoller(listen_cnt + 1)),
    pool_(ThreadPool::CreatePool(thread_nr)),
    timer_queue_(TimerQueue()),
    connector_(Connector::CreateConnector(port, this, epoller_)) {
  static_assert(epoller_ != nullptr, "create the instance of Epoller failed");
  static_assert(pool_ != nullptr, "create the instance of ThreadPool failed");
  static_assert(connector_ != nullptr, "create the instance of Connector failed");
}

void Server::Insert(std::pair<int, Channal *> &channal) {
  channal_map_.insert(std::move(channal));
}

void Server::Erase(int fd) {
  channal_map_.erase(fd);
}

bool Server::doevent(struct epoll_event* _epollevents, int eventsnum) {
  requestmsg* req = nullptr;

  for (int i = 0; i < eventsnum; ++i) {
    req = static_cast<requestmsg*>(_epollevents[i].data.ptr);
    if (req->getfd() == LSocket) {
      auto ret = connection();
      if (!ret) {
        print("error message accept connection fail .continue .........");
      }
    } else {
      if ((_epollevents[i].events & EPOLLERR) ||
          (_epollevents[i].events & EPOLLHUP) ||
          (!(_epollevents[i].events & EPOLLIN))) {
        auto events = _epollevents[i].events;
        print("error message the opposite fd  had shutdown the connection", events);
        close(req->getfd());
        delete req;
        continue;
      }

      Pool->push(requestmsg::handle, EpollFd, req);
    }
  }

  return true;
}

void Server::start() {
  int event_nr;
  struct epoll_event *event;

  while (true) {
    std::tuple<int, struct epoll_event *> ret = epoller_.EpollWait(EPOLL_WAIT_BLOCK);
    event_nr = std::get<0>(ret);
    event = std::get<1>(ret);

    if (event_nr == 0) {
      LOG_ERROR("invalid epoll wait ret value, ignore it");
      continue;
    }
    LOG_DEBUG("epoll wait get events about %d", event_nr);
    
    
  }
}


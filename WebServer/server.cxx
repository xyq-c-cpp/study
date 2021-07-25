/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of web server class, a single class.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <epoller.h>
#include <threadpool.h>
#include <connector.h>
#include <timer.h>
#include <channal.h>
#include <server.h>

Server *Server::CreateServer(unsigned int port, int thread_nr, int listen_cnt) {
  static Server *tmp = nullptr;
  if (!tmp) {
    tmp = new Server(port, thread_nr, listen_cnt);
    assert(tmp != nullptr);
  }

  return tmp;
}

Server::Server(int port, int thread_nr, int listen_cnt)
  : port_(port),
    epoller_(Epoller::CreateEpoller(listen_cnt + 1, this)),
    pool_(ThreadPool::CreatePool(thread_nr)),
    timer_queue_(new TimerQueue()),
    connector_(Connector::CreateConnector(port, this, epoller_, listen_cnt)) {
  assert(epoller_ != nullptr);
  assert(pool_ != nullptr);
  assert(connector_ != nullptr);
}

void Server::Insert(std::pair<int, std::shared_ptr<Channal> > channal) {
  channal_map_.insert(std::move(channal));
}

void Server::Erase(int fd) {
  channal_map_.erase(fd);
}

void Server::EventProc(Task callback) {
  pool_->Insert(std::function<int()>(callback));
}

void Server::Start(void) {
  int event_nr;
  struct epoll_event *event;

  while (true) {
    epoller_->EpollWait(EPOLL_WAIT_BLOCK);

    
  }
}


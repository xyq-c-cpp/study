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
    epoller_(Epoller::CreateEpoller(listen_cnt + 1)),
    pool_(ThreadPool::CreatePool(thread_nr)),
    timer_queue_(new TimerQueue()),
    connector_(std::shared_ptr<Connector>(
      Connector::CreateConnector(port, epoller_, listen_cnt))) {
  assert(pool_ != nullptr);
  assert(connector_.get() != nullptr);
}

void Server::Init(void) {
  pool_->Init();
  
  int ret = epoller_->AddReadEvent(connector_->Fd(), std::bind(&Connector::Connect, 
    connector_, epoller_), EPOLLIN | EPOLLET);
  assert(ret == 0);

  epoller_->SetOwn(this);

  connector_->SetOwn(this);

  timer_queue_->AddTimer(std::bind(&Log::UpdateTime, g_logger), 1, 0, true);
}

void Server::InsertChannal(std::pair<int, std::shared_ptr<Channal> > channal) {
  channal_map_.insert(std::move(channal));
}

void Server::EraseChannal(int fd) {
  channal_map_.erase(fd);
}

void Server::TaskInQueue(Task callback) {
  pool_->InsertTask(std::move(callback));
}

void Server::Start(void) {
  int flag;

  while (true) {
    timer_queue_->TimerProc();

    flag = timer_queue_->GetTimeOfLateTimeOut();

    LOG_DEBUG("The time of epoll_wait : %d", flag);
 
    epoller_->EpollWait(flag);
  }
}


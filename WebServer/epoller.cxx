/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the class of epoller, wraping epoll
 */

#include <epoller.h>
#include <server.h>

Epoller::Epoller(unsigned int event_num, Server *server) 
    : event_num_(event_num), 
      fd_(epoll_create(event_num_)),
      server_(server),
      event_arr_(std::vector<struct epoll_event>(event_num_)) {
  assert(fd_ >= 0);
}

Epoller *Epoller::CreateEpoller(unsigned int event_num, Server *server) {
  static Epoller *tmp = nullptr;

  assert(server != nullptr);
  if (!tmp) {
    tmp = new Epoller(event_num, server);
    assert(tmp != nullptr);
  }

  return tmp;
}

void Epoller::DelFd(int fd) {
  unsigned int event = EPOLLIN | EPOLLOUT;
  EpollCtl(EPOLL_CTL_DEL, fd, event, NULL);
  fd_event_.erase(fd);
  event_callbck_.erase(fd);
}

Epoller::~Epoller() {
  close(fd_);
}

int Epoller::EpollCtl(int flag, int fd, int event, void *arg) {
  event_.data.ptr = arg;
  event_.events = event;

  return epoll_ctl(fd_, flag, fd, &event_);
}

int Epoller::AddReadEvent(int fd, std::function<int(Epoller *)> callback) {
  unsigned int event = EPOLLIN | EPOLLET | EPOLLONESHOT;

  if (fd_event_.find(fd) == fd_event_.end()) {
    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(callback), 
      std::function<int(Epoller *)>())));
    goto end;
  } else {
    event = fd_event_[fd];
    if (event & EPOLLIN) { /* it should be the second time to call */
      goto end;
    }
    event |= EPOLLIN;
    fd_event_[fd] = event;
    Callback tmp = event_callbck_[fd];
    tmp.SetReadCallback(std::move(callback));
    event_callbck_.erase(fd);
    event_callbck_.insert(std::make_pair(fd, tmp));
  }

end:
  return EpollCtl(EPOLL_CTL_MOD, fd, event, NULL);
}

int Epoller::AddWriteEvent(int fd, std::function<int(Epoller *)> callback) {
  unsigned int event = EPOLLOUT | EPOLLET | EPOLLONESHOT;

  if (fd_event_.find(fd) == fd_event_.end()) {
    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(callback), 
      std::function<int(Epoller *)>())));
    goto end;
  } else {
    event = fd_event_[fd];
    if (event & EPOLLOUT) {
      goto end;
    }
    event |= EPOLLOUT;
    fd_event_[fd] = event;
    Callback tmp = event_callbck_[fd];
    tmp.SetWriteCallback(std::move(callback));
    event_callbck_.erase(fd);
    event_callbck_.insert(std::make_pair(fd, tmp));
  }

end:
  return EpollCtl(EPOLL_CTL_MOD, fd, event, NULL);
}

int Epoller::AddReadWriteEvent(int fd, std::function<int(Epoller *)> read_cb, 
    std::function<int(Epoller *)> write_cb) {
  unsigned int event = EPOLLOUT | EPOLLOUT | EPOLLET | EPOLLONESHOT;

  if (fd_event_.find(fd) == fd_event_.end()) {
    LOG_DEBUG("The first time to add read-write event, fd %d", fd);
    fd_event_.insert(std::make_pair(fd, event));
    event_callbck_.insert(std::make_pair(fd, Callback(std::move(read_cb), 
      std::move(write_cb))));
    goto end;
  } else {
    event = fd_event_[fd];
    if (event & EPOLLIN && event & EPOLLOUT) {
      goto end;
    }
    event |= EPOLLIN | EPOLLOUT;
    fd_event_[fd] = event;
    Callback tmp = event_callbck_[fd];
    tmp.SetWriteCallback(std::move(write_cb));
    tmp.SetReadCallback(std::move(read_cb));
    event_callbck_.erase(fd);
    event_callbck_.insert(std::make_pair(fd, tmp));
  }

end:
  return EpollCtl(EPOLL_CTL_ADD, fd, event, NULL);
}

void Epoller::EpollWait(int timeout) {
  int ret;
  int callback_ret;
  unsigned int event;

  ret = epoll_wait(fd_, &event_arr_[0], static_cast<int>(event_num_), timeout);
  if (ret <= 0) {
    LOG_WARN("epoll_wait ret <= 0, maybe invalid events");
    return;
  }

  for (int i = 0; i < ret; ++i) {
    if (event_arr_[i].events & EPOLLIN) {
      callback_ret = event_callbck_[event_arr_[i].data.fd].read_callback_(this);
      LOG_DEBUG("fd read callbck ret %d, fd %d", callback_ret, \
        event_arr_[i].data.fd);
    } else if (event_arr_[i].events & EPOLLOUT) {
      callback_ret = event_callbck_[event_arr_[i].data.fd].write_callback_(this);
      LOG_DEBUG("fd write callbck ret %d, fd %d", callback_ret, \
        event_arr_[i].data.fd);
    } else {
      LOG_DEBUG("No expected event %d, fd %d, close it", event_arr_[i].events, \
        event_arr_[i].data.fd);
      DelFd(event_arr_[i].data.fd);
      server_->Erase(event_arr_[i].data.fd);
      (void)close(event_arr_[i].data.fd);
      continue;
    }
  }
}


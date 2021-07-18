/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: common function
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <sys/epoll.h>
#include <fcntl.h>
#include <iostream>

#include <common.h>

bool web_svr_set_fd_no_block(int32_t fd) {
  int flag = fcntl(fd, F_GETFL, 0);
  if (flag == -1) {
    print("error message set noblocksocket1 fail ,connection fail !");
    return false;
  }

  flag |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flag) == -1) {
    print("error message set noblocksocket2 fail ,connection fail !");
    return false;
  }
  print("set noblock successs , fd : ", fd);

  return true;
}

bool web_svr_epoll_ctl(int32_t epoll_fd, int32_t flag, void *req,
      int32_t fd, __int32_t events) {
  struct epoll_event event;

  event.data.ptr = req;
  event.events = events;

  if (epoll_ctl(epoll_fd, flag, fd, &event) < 0) {
    print("error message : epoll_ctl fail ! _epollfd : ", epoll_fd,
      " clientfd : ", fd);
    return false;
  }
  print("epoll_ctl successs , _epollfd : ", epoll_fd, "clientfd : ", fd);

  return true;
}

int32_t web_svr_read(int32_t fd, uint8_t *buf, uint32_t len) {
  uint32_t sizetoread = len;
  uint32_t sum = 0, now_read = 0;

  while (sizetoread > 0) {
    now_read = read(fd, buf + sum, sizetoread);
    if (now_read < 0) {
      if (errno == EINTR) {
        now_read = 0;
        continue;
      } else if (errno == EAGAIN) {
        return sum;
      } else {
        return -1;
      }
    } else if (now_read == 0) {
      break;
    }
    sum += now_read;
    sizetoread -= now_read;
  }

  return sum;
}

int32_t web_svr_write(int32_t fd, uint8_t buf, uint32_t len) {
  size_t sizetowrite = len;
  ssize_t sum = 0 , written = 0;

  while (sizetowrite > 0) {
    written = write(fd, buf + sum, sizetowrite);
    if (written < 0) {
      if(errno == EINTR || errno == EAGAIN) {
        continue;
      } else {
        return -1;
      }
    }

    sizetowrite -= written;
    sum += written;
  }

  return sum;
}


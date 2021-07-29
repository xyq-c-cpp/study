/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: common function
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <common.h>

bool web_svr_set_fd_no_block(int fd) {
  int flag = fcntl(fd, F_GETFL, 0);
  if (flag == -1) {
    LOG_ERROR("fcntl get failed, fd %d", fd);
    return false;
  }

  flag |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flag) == -1) {
    LOG_ERROR("fcntl set failed, fd %d", fd);
    return false;
  }
  LOG_DEBUG("fcntl fd %d success", fd);

  return true;
}

int web_svr_read(int fd, char *buf, int len) {
  int sizetoread = len;
  int sum = 0, now_read = 0;

  while (sizetoread > 0) {
    now_read = read(fd, buf + sum, sizetoread);
    if (now_read < 0) {
      /* No block mode, we shall try again */
      if (errno == EINTR) {
        continue;
      } else if (errno == EAGAIN || errno == EWOULDBLOCK )
        return sum;
      else {
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

int web_svr_write(int fd, char *buf, int len) {
  int sizetowrite = len;
  int sum = 0 , written = 0;

  while (sizetowrite > 0) {
    written = write(fd, buf + sum, sizetowrite);
    if (written < 0) {
      /* No block mode, we shall try again */
      if(errno == EINTR) {
        continue;
      } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return sum;
      } else {
        return -1;
      }
    }

    sizetowrite -= written;
    sum += written;
  }

  return sum;
}


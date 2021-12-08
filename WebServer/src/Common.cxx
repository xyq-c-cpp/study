/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: common function
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <Common.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

bool web_svr_set_fd_no_block(int fd) {
  int flag = fcntl(fd, F_GETFL, 0);
  if (flag == -1)
    return false;

  flag |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flag) == -1)
    return false;
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
      } else if (errno == EAGAIN || errno == EWOULDBLOCK)
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
  int sum = 0, written = 0;

  while (sizetowrite > 0) {
    written = write(fd, buf + sum, sizetowrite);
    if (written < 0) {
      /* No block mode, we shall try again */
      if (errno == EINTR) {
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

void setSocketNoLinger(int fd) {
  struct linger linger_;
  linger_.l_onoff = 1;
  linger_.l_linger = 30;
  (void)setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&linger_,
                   sizeof(linger_));
}

void setSocketNodelay(int fd) {
  int enable = 1;
  (void)setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable,
                   sizeof(enable));
}

void handleSigpipe() {
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = SIG_IGN;
  (void)sigaction(SIGPIPE, &act, nullptr);
}

int getMaxOpenFileNum() {
  struct rlimit r;
  int ret = getrlimit(RLIMIT_NOFILE, &r);
  if (ret != 0) {
#ifdef DEBUG
    std::cout << "getrlimit failed, ret " << ret << " errstr "
              << strerror(errno) << std::endl;
#endif
    return 1024;
  }
#ifdef DEBUG
  std::cout << "max open file " << static_cast<int>(r.rlim_max) << std::endl;
#endif
  return static_cast<int>(r.rlim_max);
}

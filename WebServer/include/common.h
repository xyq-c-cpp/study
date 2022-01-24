/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: common function
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef _MSC_VER
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#else

typedef int mode_t;
typedef int nlink_t;
typedef int uid_t;
typedef int gid_t;
typedef int blksize_t;
typedef int blkcnt_t;

struct stat {
  mode_t st_mode;       //文件对应的模式，文件，目录等
  ino_t st_ino;         // inode节点
  dev_t st_dev;         //设备号码
  dev_t st_rdev;        //特殊设备号码
  nlink_t st_nlink;     //文件的连接数
  uid_t st_uid;         //文件所有者
  gid_t st_gid;         //文件所有者对应的组
  off_t st_size;        //普通文件，对应的文件字节
  time_t st_atime;      //文件最后被访问的时间
  time_t st_mtime;      //文件内容最后被修改的时间
  time_t st_ctime;      //文件状态改变时间
  blksize_t st_blksize; //文件内容对应的块大小
  blkcnt_t st_blocks;   //伟建内容对应的块数量
};

struct linger {
  int l_onoff;
  int l_linger;
};

typedef int siginfo_t;
typedef int sigset_t;
struct sigaction {
  void (*sa_handler)(int);
  void (*sa_sigaction)(int, siginfo_t *, void *);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_restorer)(void);
};

struct rlimit {
  unsigned int rlim_cur;
  unsigned int rlim_max;
};

typedef std::uint32_t __uint32_t;
typedef std::uint64_t __uint64_t;
typedef union epoll_data {
  void *ptr;
  int fd;
  __uint32_t u32;
  __uint64_t u64;
} epoll_data_t;

struct epoll_event {
  __uint32_t events;
  epoll_data_t data;
};

struct sockaddr_in {
  short int sin_family;
  unsigned short int sin_port;
  struct in_addr {
    unsigned long s_addr;
  };
  struct in_addr sin_addr;
  unsigned char sin_zero[8];
};

struct sockaddr {};

struct socklen_t {};

int send(int, const char *, int, int);
int open(const char *, int, int);
void *mmap(void *, int, int, int, int, int);
int munmap(void *, int);
int close(int);
int fcntl(int, int, int);
int read(int, const char *, int);
int write(int, const char *, int);
int setsockopt(int, int, int, const void *, int);
int sigaction(int, struct sigaction *, void *);
int getrlimit(int, struct rlimit *);
int epoll_create(int);
int epoll_add(int, int, int);
int epoll_ctl(int, int, int, struct epoll_event *);
int epoll_wait(int, struct epoll_event *, int, int);
int eventfd(int, int);
int pthread_setname_np(std::thread::native_handle_type, const char *);
int getopt(int, const char **, const char *);
int socket(int, int, int);
int htons(unsigned int);
int htonl(int);
int bind(int, struct sockaddr *, int);
int listen(int, int);
int accept(int, struct sockaddr *, socklen_t *);
char *inet_ntoa(struct sockaddr_in::in_addr);
int recv(int, const char *, int, 0);

extern char *optarg;

#define INADDR_ANY 1
#define SO_REUSEADDR 1
#define SOCK_STREAM 1
#define PROT_READ 1
#define MAP_PRIVATE 1
#define EPOLLIN 1
#define EPOLLET 1
#define EPOLLONESHOT 1
#define EPOLLOUT 1
#define EPOLLHUP 1
#define EPOLLERR 1
#define EPOLLPRI 1
#define F_GETFL 1
#define O_NONBLOCK 1
#define F_SETFL 1
#define SOL_SOCKET 1
#define SO_LINGER 1
#define IPPROTO_TCP 1
#define TCP_NODELAY 1
#define SIGPIPE 1
#define RLIMIT_NOFILE 1
#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_MOD 1
#define EPOLL_CTL_DEL 1
#define EFD_NONBLOCK 1
#define EFD_CLOEXEC 1
#define AF_INET 1

#endif

#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <Log.h>
#include <config.h>

#define WEB_SVR_BUFF_SIZE_32 32
#define WEB_SVR_BUFF_SIZE_64 64
#define WEB_SVR_BUFF_SIZE_128 128
#define WEB_SVR_BUFF_SIZE_256 256
#define WEB_SVR_BUFF_SIZE_512 512
#define WEB_SVR_BUFF_SIZE_1024 1024
#define WEB_SVR_BUFF_SIZE_2048 2048
#define WEB_SVR_BUFF_SIZE_4096 4096

#define MAX_FD_NUM 1048575
#define MAX_RETRY_TIME 3
#define DEFAULT_TIMEOUT_MS (2 * 1000)
#define KEEP_ALIVE_TIMEOUT_S (5 * 60)
#define KEEP_ALIVE_TIMEOUT_MS (KEEP_ALIVE_TIMEOUT_S * 1000)

class Timer;
class TimerManager;
class Message;
class Channal;
class Server;
class Epoller;
class EventLoop;
class EventLoopThread;
class EventLoopThreadPool;

#include <Log.h>

#define EPOLL_WAIT_BLOCK -1
#define EPOLL_WAIT_RET_IMMEDIATE 0

typedef std::function<int(void)> Task;
typedef std::function<int(void)> EventCb;
typedef std::function<int(void)> TimerCb;

bool web_svr_set_fd_no_block(int fd);
int web_svr_read(int fd, char *buf, int len);
int web_svr_write(int fd, char *buf, int len);
void setSocketNoLinger(int fd);
void setSocketNodelay(int fd);
void handleSigpipe();
int getMaxOpenFileNum();

#endif

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

#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <iostream>
#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <map>
#include <list>
#include <sys/resource.h>

#include <config.h>

#define WEB_SVR_BUFF_SIZE_32      32
#define WEB_SVR_BUFF_SIZE_64      64
#define WEB_SVR_BUFF_SIZE_128     128
#define WEB_SVR_BUFF_SIZE_256     256
#define WEB_SVR_BUFF_SIZE_512     512
#define WEB_SVR_BUFF_SIZE_1024    1024
#define WEB_SVR_BUFF_SIZE_2048    2048
#define WEB_SVR_BUFF_SIZE_4096    4096

#define MAX_FD_NUM                1048575
#define MAX_RETRY_TIME            3
#define DEFAULT_TIMEOUT_MS        (2 * 1000)
#define KEEP_ALIVE_TIMEOUT_S      (5 * 60)
#define KEEP_ALIVE_TIMEOUT_MS     (KEEP_ALIVE_TIMEOUT_S * 1000)

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

#define EPOLL_WAIT_BLOCK          -1
#define EPOLL_WAIT_RET_IMMEDIATE  0

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


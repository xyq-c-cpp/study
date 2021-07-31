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
#include <limits.h>
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

#include <config.h>

#define WEB_SVR_BUFF_SIZE_32      32
#define WEB_SVR_BUFF_SIZE_64      64
#define WEB_SVR_BUFF_SIZE_128     128
#define WEB_SVR_BUFF_SIZE_256     256
#define WEB_SVR_BUFF_SIZE_512     512
#define WEB_SVR_BUFF_SIZE_1024    1024
#define WEB_SVR_BUFF_SIZE_2048    2048
#define WEB_SVR_BUFF_SIZE_4096    4096

class TimeSpace;
class Timer;
class TimerQueue;
class Message;
class Channal;
class Server;
class ThreadPool;
class Epoller;
class Connector;
class Log;

#include <log.h>

#define EPOLL_WAIT_BLOCK          -1
#define EPOLL_WAIT_RET_IMMEDIATE  0

typedef std::function<int(void)> Task;
typedef std::function<int(void)> EventCb;
typedef std::function<int(void)> TimerCb;

bool web_svr_set_fd_no_block(int fd);
int web_svr_read(int fd, char *buf, int len);
int web_svr_write(int fd, char *buf, int len);

#endif


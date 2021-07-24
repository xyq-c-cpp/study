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

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#define WEB_SVR_BUFF_SIZE_32      32
#define WEB_SVR_BUFF_SIZE_64      64
#define WEB_SVR_BUFF_SIZE_128     128
#define WEB_SVR_BUFF_SIZE_256     256
#define WEB_SVR_BUFF_SIZE_512     512
#define WEB_SVR_BUFF_SIZE_1024    1024
#define WEB_SVR_BUFF_SIZE_2048    2048
#define WEB_SVR_BUFF_SIZE_4096    4096

#define EPOLL_WAIT_BLOCK          -1
#define EPOLL_WAIT_RET_IMMEDIATE  0

class Log;
class TimeSpace;
class Timer;
class Message;

extern Log *g_logger;

#define LOG_DEBUG(fmt, ...) do { \
  g_logger.Logging(__FILE__, __LINE__, __FUNCTION__, DEBUG, fmt, ##__VA_ARGS__); \
} while (0)

#define LOG_WARN(fmt, ...) do { \
  g_logger.Logging(__FILE__, __LINE__, __FUNCTION__, WARN, fmt, ##__VA_ARGS__); \
} while (0)

#define LOG_ERROR(fmt, ...) do { \
  g_logger.Logging(__FILE__, __LINE__, __FUNCTION__, ERROR, fmt, ##__VA_ARGS__); \
} while (0)

int web_svr_log_init(void);
int web_svr_add_timer();
bool web_svr_set_fd_no_block(int32_t fd);
bool web_svr_epoll_ctl(int32_t epoll_fd, int32_t flag, void *req,
  int32_t fd, __int32_t events);
int32_t web_svr_read(int32_t fd, uint8_t *buf, uint32_t len);
int32_t web_svr_write(int32_t fd, uint8_t buf, uint32_t len);

#endif


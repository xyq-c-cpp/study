/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of log class, use as a single class to record message.
 * note: restructure with C++11 and code specification. This file is added as the new feature.
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <common.h>

#define DEFAULT_LOG_FILE      "/tmp/web_svr.log"

typedef enum {
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_MAX
}log_level_t;

class Log : public std::enable_shared_from_this<Log> {
 public:
  static Log *CreateLogger(log_level_t level, 
    const char *file_name = DEFAULT_LOG_FILE);
  void Logging(const char *file, unsigned int line, const char *func, 
    log_level_t level, const char *fmt, ...);
  ~Log();
  int UpdateTime(void);

 private:
  Log(const char *file_name, log_level_t level = LOG_LEVEL_WARN);
  const char *GetLogLevelStr(log_level_t level);

  log_level_t level_;
  TimeSpace *time_;
  FILE *fd_;
  char file_name_[WEB_SVR_BUFF_SIZE_64];
  std::string buff_;

  std::mutex lock_;
};

extern std::shared_ptr<Log> g_logger;

#ifndef DEBUG
#define LOG_DEBUG(fmt, ...) do { \
  g_logger->Logging(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_DEBUG, fmt"\n", \
    ##__VA_ARGS__); \
} while (0)

#define LOG_WARN(fmt, ...) do { \
  g_logger->Logging(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_WARN, fmt"\n", \
    ##__VA_ARGS__); \
} while (0)

#define LOG_ERROR(fmt, ...) do { \
  g_logger->Logging(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_ERROR, fmt"\n", \
    ##__VA_ARGS__); \
} while (0)

#else 

/* va_list use at function, __VA_ARGS__ use at macro defination */
void log(const char *file, int line, const char *func, const char *fmt, ...);

#define LOG_DEBUG(fmt, ...) do {\
  log(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
 } while (0)

#define LOG_WARN(fmt, ...) do {\
  log(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
 } while (0)

#define LOG_ERROR(fmt, ...) do {\
  log(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__); \
 } while (0)

#endif

#endif /* _LOG_H_ */


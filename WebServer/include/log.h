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

class Log {
 public:
  static Log *CreateLogger(char *file_name = DEFAULT_LOG_FILE);
  void Logging(const char *file, unsigned int line, const char *func, 
    log_level_t level, const char *fmt, ...);

 private:
  Log(char *file_name, log_level_t level = LOG_LEVEL_WARN);
  ~Log();
  const char *GetLogLevelStr(log_level_t level);
  void UpdateTime(void);

  log_level_t level_;
  TimeSpace *time_;
  FILE *fd_;
  char file_name_[WEB_SVR_BUFF_SIZE_64];
  std::string buff_;

  std::mutex lock_;
};

extern Log *g_logger;

#define LOG_DEBUG(fmt, ...) do { \
  g_logger->Logging(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_DEBUG, fmt"\n", ##__VA_ARGS__); \
} while (0)

#define LOG_WARN(fmt, ...) do { \
  g_logger->Logging(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_WARN, fmt"\n", ##__VA_ARGS__); \
} while (0)

#define LOG_ERROR(fmt, ...) do { \
  g_logger->Logging(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_ERROR, fmt"\n", ##__VA_ARGS__); \
} while (0)

#endif /* _LOG_H_ */


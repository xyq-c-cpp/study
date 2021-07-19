/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of log class, use as a single class to record message.
 * note: restructure with C++11 and code specification. This file is added as the new feature.
 */

#ifndef _LOG_H_
#define _LOG_H_

class TimeSpace;

#define DEFAULT_LOG_FILE      "/tmp/web_svr.log"

enum log_level_t{
  DEBUG = 0,
  WARN,
  ERROR,
  LOG_LEVEL_MAX,
};

typedef enum log_level_t log_level_t;

class Log {
 public:
  static Log *CreateLogger(uint8_t *file_name = DEFAULT_LOG_FILE);
  void Logging(const char *func, uint32_t line, log_level_t level, 
    const char *fmt, ...);

 private:
  Log(uint8_t *file_name, log_level_t level = WARN);
  ~Log();
  const char *GetLogLevelStr(void);
  void UpdateTime(void);

  log_level_t level_;
  TimeSpace time_;
  FILE *fd_;
  uint8_t file_name_[WEB_SVR_BUFF_SIZE_64];
  std::string buff_;

  std::mutex lock_;
};

#endif /* _LOG_H_ */


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
#define LOG_BUFF_BLOCK_NR     4
#define LOG_BUFF_BLOCK_SIZE   4096

typedef enum {
  DEBUG = 0,
  WARN,
  ERROR,
  LOG_LEVEL_MAX,
}log_level_t;

class Log {
 public:
  static Log *CreateLogger(uint8_t *file_name = nullptr);
  void Logging(const char *func, uint32_t line, log_level_t level, const char *fmt, ...);

 private:
  Log(uint8_t *file_name = nullptr, log_level_t level = WARN);
  ~Log();
  void UpdateTime(void);

  log_level_t level_;
  TimeSpace time_;
  FILE *fd_;
  uint8_t file_name_[WEB_SVR_BUFF_SIZE_64];
  uint8_t *buf_[LOG_BUFF_BLOCK_NR];
  uint8_t cur_input_, cur_ouput_;
  uint16_t cur_input_pos_;
  uint16_t cur_output_pos_;
};

#endif /* _LOG_H_ */


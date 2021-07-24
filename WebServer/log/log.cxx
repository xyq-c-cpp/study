/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of log class, use as a single class to record message.
 * note: restructure with C++11 and code specification. This file is added as the new feature.
 */

#include <log.h>

Log *g_logger = Log::CreateLogger();

const char *log_level2str[] = {
  "DEBUG",
  "WARN",
  "ERROR"
};

Log *Log::CreateLogger(uint8_t *file_name) {
  static Log *logger = nullptr;

  if (!logger) {
    logger = new Log(file_name);
    static_assert(logger != nullptr, "new log instance failed");
  }

  return logger;
}

Log::Log(uint8_t *file_name, log_level_t level)
    : level_(level),
      time_(TimeSpace()) {
  static_assert(file_name != nullptr, "file_name is nullptr");
  strncpy(file_name_, file_name, sizeof(file_name_));

  fd_ = fopen(file_name_, "a+");
  static_assert(fd_ != nullptr, "open fd_ failed.");
}

Log::~Log() {
  if (fd_) {
    (void)fclose(fd_);
  }
}

const char *Log::GetLogLevelStr(log_level_t level) {
  return const_cast<const char *>(log_level2str[static_cast<int>(level)]);
}

void Log::Logging(const char *file, uint32_t line, const char *func, 
    log_level_t level, const char *fmt, ...) {
  /* a single log is less than 256 bytes */
  char tmp[WEB_SVR_BUFF_SIZE_256] = {0};
  va_list va;

  if (level < level_ || level >= LOG_LEVEL_MAX) {
    return;
  }
  
  static_assert(func != nullptr, "func is nullptr");

  va_start(fmt, va);
  vsnprintf(tmp, sizeof(tmp), "%s %s(%d) %-8s %s %s\n", time_.GetTimeStr(), file,
    line, GetLogLevelStr(level), func, va);
  va_end(va);

  std::lock_guard<std::mutex> local_lock(lock_);
  buff_ += tmp;

#ifdef DEBUG
  (void)web_svr_write(fileno(fd_), static_cast<uint8_t *>(buff_.c_str()), 
    buff_.size());
  buff_.clear();
#endif
}

void Log::UpdateTime(void) {
  uint32_t sum, num;

  sum = buff_.size();
  num = web_svr_write(fileno(fd_), static_cast<uint8_t *>(buff_.c_str()), 
    sum);
  if (num != buff_size()) {
    buff_.clear();
    Logging(__FILE__, __LINE__, __FUNCTION__, ERROR, "the size of writing dismatch"
      ", all num %d, had write num %d", sum, num);
  }
  buff_.clear();
  (void)gettimeofday(&time_, NULL);
  fflush(fd_);
}


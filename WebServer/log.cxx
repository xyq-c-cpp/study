/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of log class, use as a single class to record message.
 * note: restructure with C++11 and code specification. This file is added as the new feature.
 */

#include <log.h>
#include <timer.h>
#include <config.h>

#ifdef DEBUG

void log(const char *file, int line, const char *func, const char *fmt, ...) {
  char buf[1024] = {0};
  char time[48] = {0};
  struct timeval timeval;

  gettimeofday(&timeval, NULL);
  strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", localtime(&timeval.tv_sec));
  sprintf(time, "%s %03ld", time, timeval.tv_usec / 1000);
  int num = sprintf(buf, "%s %d %s ", strrchr(file, '/') + 1, line, func);
  va_list va;
  va_start(va, fmt);
  vsprintf(buf + num, fmt, va);
  va_end(va);
  printf("%s %s\n", time, buf);
}

#endif

Log *g_logger = Log::CreateLogger();

const char *log_level2str[] = {
  "DEBUG",
  "WARN",
  "ERROR"
};

Log *Log::CreateLogger(char*file_name) {
  static Log *logger = nullptr;

  if (!logger) {
    logger = new Log(file_name);
    assert(logger != nullptr);
  }

  return logger;
}

Log::Log(char *file_name, log_level_t level)
    : level_(level),
      time_(new TimeSpace()) {
  assert(file_name != nullptr);
  strncpy(file_name_, file_name, sizeof(file_name_));
  assert(time_ != nullptr);

  fd_ = fopen(file_name_, "a+");
  assert(fd_ != nullptr);
}

Log::~Log() {
  if (fd_) {
    (void)fclose(fd_);
  }
}

const char *Log::GetLogLevelStr(log_level_t level) {
  return const_cast<const char *>(log_level2str[static_cast<int>(level)]);
}

void Log::Logging(const char *file, unsigned int line, const char *func, 
    log_level_t level, const char *fmt, ...) {
  /* a single log is less than 256 bytes */
  char tmp[WEB_SVR_BUFF_SIZE_256] = {0};
  va_list va;
  int num;

  if (level < level_ || level >= LOG_LEVEL_MAX) {
    return;
  }
  
  assert(func != nullptr);

  num = snprintf(tmp, sizeof(tmp) - 1, "%s %s(%d) %-8s %s ", time_->GetTimeStr(), 
    strrchr(file, '/') + 1, line, GetLogLevelStr(level), func);

  va_start(va, fmt);
  num += vsnprintf(tmp + num, sizeof(tmp) - num - 1, fmt, va);
  va_end(va);

  std::lock_guard<std::mutex> local_lock(lock_);
  buff_ += tmp;

#ifdef DEBUG
  (void)web_svr_write(fileno(fd_), const_cast<char *>(buff_.c_str()), 
    buff_.size());
  buff_.clear();
  std::cout << tmp;
#endif
}

void Log::UpdateTime(void) {
  unsigned int sum, num;

  sum = buff_.size();
  num = web_svr_write(fileno(fd_), const_cast<char*>(buff_.c_str()), 
    sum);
  if (num != buff_.size()) {
    buff_.clear();
    Logging(__FILE__, __LINE__, __FUNCTION__, LOG_LEVEL_ERROR, "the size of writing"
      " dismatch, all num %d, had write num %d", sum, num);
  }
  buff_.clear();
  (void)gettimeofday(time_->GetTimePtr(), NULL);
  fflush(fd_);
}


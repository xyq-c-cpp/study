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
  static char buf[WEB_SVR_BUFF_SIZE_1024] = {0};
  static char time[WEB_SVR_BUFF_SIZE_64] = {0};
  static struct timeval timeval;
  int num;

  gettimeofday(&timeval, NULL);
  strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", localtime(&timeval.tv_sec));
  sprintf(time, "%s %03ld", time, timeval.tv_usec / 1000);
  num = sprintf(buf, "%s %d %s ", strrchr(file, '/') + 1, line, func);
  va_list va;
  va_start(va, fmt);
  vsprintf(buf + num, fmt, va);
  va_end(va);
  printf("%s %s\n", time, buf);
}

#endif

std::shared_ptr<Log> g_logger(Log::CreateLogger(LOG_LEVEL_DEBUG));

const char *log_level2str[] = {
  "DEBUG",
  "WARN",
  "ERROR"
};

Log *Log::CreateLogger(log_level_t level, const char *file_name) {
  static Log *logger = nullptr;

  if (!logger) {
    logger = new Log(file_name, level);
    assert(logger != nullptr);
  }

  return logger;
}

Log::Log(const char *file_name, log_level_t level)
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

  if (time_) {
    delete time_;
  }
}

const char *Log::GetLogLevelStr(log_level_t level) {
  return const_cast<const char *>(log_level2str[static_cast<int>(level)]);
}

void Log::Logging(const char *file, unsigned int line, const char *func, 
    log_level_t level, const char *fmt, ...) {
  static char tmp[WEB_SVR_BUFF_SIZE_256] = {0};
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

  std::unique_lock<std::mutex> lock(lock_);
  buff_.append(tmp);
}

int Log::UpdateTime(void) {
  std::string tmp;

  if (buff_.size()) {
    std::unique_lock<std::mutex> lock(lock_);
    tmp = buff_;
    buff_.clear();
  } else {
    LOG_DEBUG("No log message to write at log file....");
  }

  if (tmp.size() > 0) {
    LOG_DEBUG("the logger has %d bytes stream to write to log file", tmp.size());
 
    (void)web_svr_write(fileno(fd_), const_cast<char*>(buff_.c_str()), 
      tmp.size());
    fflush(fd_);
  }
  
  (void)gettimeofday(time_->GetTimePtr(), nullptr);

  return 0;
}


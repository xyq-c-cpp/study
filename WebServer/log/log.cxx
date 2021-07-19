/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of log class, use as a single class to record message.
 * note: restructure with C++11 and code specification. This file is added as the new feature.
 */

Log *Log::CreateLogger(uint8_t *file_name) {
  static Log *logger = nullptr;

  if (!logger) {
    logger = new Log(file_name);
    static_assert(logger != nullptr, "new log instance failed");
  }

  return logger;
}

Log::Log(uint8_t *file_name, log_level_t level)
    : level_(level), time_(TimeSpace()){
  if (file_name) {
    strncpy(file_name_, file_name, sizeof(file_name_));
  } else {
    strncpy(file_name_, DEFAULT_LOG_FILE, sizeof(file_name_));
  }

  fd_ = fopen(file_name_, "a+");
  static_assert(fd_ != nullptr, "open fd_ failed.");

  for (int i = 0; i < sizeof(buf_)/sizeof(void *); ++i) {
    buf[i] = operator new(LOG_BUFF_BLOCK_SIZE);
    static_assert(buf_[i] != nullptr, "new memory failed");
  }
  cur_input_ = cur_ouput_ = cur_input_pos_ = cur_output_pos_ = 0;
}

Log::~Log() {
  if (fd_) {
    (void)fclose(fd_);
  }

  for (int i = 0; i < sizeof(buf_)/sizeof(void *); ++i) {
    delete buf[i];
  }
}

void Log::Logging(const char *func, uint32_t line, log_level_t level, const char *fmt, ...) {
  static_assert(func != nullptr, "func is nullptr");
  for (; ;) {
    snprintf(&buf_[cur_input_] + cur_input_pos_, &buf_[cur_input_] + cur_input_pos_, "%s")
  }
}

void Log::UpdateTime(void) {
  
}


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

Log::Log(uint8_t *file_name, log_level_t level, uint16_t buff_lock_size, uint8_t buff_block_nr)
    : level_(level),
      time_(TimeSpace()), 
      buff_block_size_(buff_lock_size)
      buff_block_nr_(buff_block_nr) {
  if (file_name) {
    strncpy(file_name_, file_name, sizeof(file_name_));
  } else {
    strncpy(file_name_, DEFAULT_LOG_FILE, sizeof(file_name_));
  }

  fd_ = fopen(file_name_, "a+");
  static_assert(fd_ != nullptr, "open fd_ failed.");

  for (int i = 0; i < buff_block_nr_; ++i) {
    buf_[i] = operator new(LOG_BUFF_BLOCK_SIZE);
    static_assert(buf_[i] != nullptr, "new memory failed");
  }
  
  cur_input_ = cur_ouput_ = 0;
  cur_input_pos_ = cur_output_pos_ = buf_[0];

  input_nr_ = buff_block_size_ * buff_block_nr_;
  output_nr_ = 0;
}

Log::~Log() {
  if (fd_) {
    (void)fclose(fd_);
  }

  for (int i = 0; i < buff_block_nr_; ++i) {
    delete buf_[i];
  }
}

void Log::Logging(const char *file, uint32_t line, const char *func, log_level_t level, 
    const char *fmt, ...) {
  uint32_t num;
  va_list va;

  static_assert(func != nullptr, "func is nullptr");
  if (!input_nr_) {
    return;
  }
  
  va_start(fmt, va);
  for (; ;) {
    cur_input_pos_ += vsnprintf(cur_input_pos_, buff_block_size_ - (cur_input_pos_ - 
      buf_[cur_input_]), "%s(%d) %s %s", file, line, func, va);
    
  }
}

void Log::UpdateTime(void) {
  
}


#pragma once

#include <string>
#include <config.h>

template<size_t size>
class Buffer {
public:
  Buffer() {
    memset(buff_, 0, sizeof(buff_));
    size_ = 0;
    end_ = size + 1;
    cap_ = size;
  }
  ~Buffer() = default;
  size_t find(char t, int begin = 0) {
    if (begin > size_) {
      return end_;
    }
    char* tmp = buff_ + begin;
    while (tmp < buff_ + size_) {
      if (*tmp == t) {
        return tmp - buff_;
      }
      ++tmp;
    }
    return end_;
  }

  void append(char* begin, size_t len) {
    size_t t = cap_ - size_ ? len : cap_ - size_;
    memcpy(buff_ + size_, begin, t);
    size_ += t;
  }

  inline char* c_str() {
    return (buff_);
  }

  inline size_t length() {
    return size_;
  }

  inline void clear() {
    buff_[0] = '\0';
    size_ = 0;
#ifdef DEBUG
    //·Ç±ØÒª
    memset(buff_, 0, sizeof(buff_));
#endif
  }

  inline size_t cap() {
    return cap_;
  }

  inline size_t end() {
    return end_;
  }

  inline bool empty() {
    return size_ == 0;
  }

  inline void setSize(size_t s) {
    size_ = s;
  }

  std::string substr(size_t begin, size_t len = 0) {
    if (begin > size_) {
      return std::string();
    }
    if (len) {
      return std::string(&buff_[begin],
        begin + len >= size_ ?
        size_ - begin + 1 : len);
    }
    else {
      return std::string(&buff_[begin],
        size_ - begin + 1);
    }
  }

private:
  char buff_[size];
  size_t size_;
  size_t cap_;
  size_t end_;
};


#pragma once

#include <config.h>
#include <cstring>
#include <string>

template <size_t size> class Buffer {
public:
  explicit Buffer()
      : buff_(new char[size]), size_(0), cap_(size), end_(size + 1) {
    memset(buff_, 0, sizeof(cap_));
  }

  ~Buffer() {
    if (buff_) {
      delete[] buff_;
    }
  }

  Buffer(const Buffer &a) { construct(a); }

  Buffer &operator=(const Buffer &a) {
    construct(a);
    return *this;
  }

  Buffer(Buffer &&a) { remove(std::move(a)); }

  Buffer &operator=(Buffer &&a) {
    remove(std::move(a));
    return *this;
  }

  size_t find(char t, int begin = 0) {
    if (begin > size_) {
      return end_;
    }
    char *ret = strchr(buff_ + begin, t);
    if (ret != nullptr)
      return (ret - buff_);
    else
      return end_;
  }

  void append(char *begin, size_t len) {
    size_t t = cap_ - size_ ? len : cap_ - size_;
    memcpy(buff_ + size_, begin, t);
    size_ += t;
  }

  inline char *c_str() { return (buff_); }

  inline size_t length() { return size_; }

  inline void clear() {
    buff_[0] = '\0';
    size_ = 0;
    // No necessary
#ifdef DEBUG
    memset(buff_, 0, cap_);
#endif
  }

  inline size_t cap() { return cap_; }

  inline size_t end() { return end_; }

  inline bool empty() { return size_ == 0; }

  inline void setSize(size_t s) { size_ = s; }

  std::string substr(size_t begin, size_t len = 0) {
    if (begin > size_) {
      return std::string();
    }
    if (len) {
      return std::string(&buff_[begin],
                         begin + len >= size_ ? size_ - begin + 1 : len);
    } else {
      return std::string(&buff_[begin], size_ - begin + 1);
    }
  }

private:
  void construct(const Buffer &a) {
    buff_ = new char[a.cap_];
    cap_ = a.cap_;
    end_ = a.end_;
    size_ = a.size_;
    memcpy(buff_, a.buff_, size_);
  }

  void remove(Buffer &&a) {
    buff_ = a.buff_;
    size_ = a.size_;
    cap_ = a.cap_;
    end_ = a.end_;
    a.reset();
  }
  void reset() {
    buff_ = nullptr;
    size_ = 0;
    cap_ = 0;
    end_ = 0;
  }

private:
  char *buff_;
  size_t size_;
  size_t cap_;
  size_t end_;
};

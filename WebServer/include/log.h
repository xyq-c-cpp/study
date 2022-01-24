#ifndef _LOG_H_
#define _LOG_H_

#include <chrono>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <thread>

int log_init(std::string file);
void log(std::string &str);

class logger {
public:
  std::string buf;
  logger() { buf.clear(); }
  ~logger() {
    if (buf.empty())
      return;

    buf += "\n";
    log(buf);
  }

  logger &operator<<(int o) {
    addTime();
    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(double o) {
    addTime();
    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(float o) {
    addTime();
    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(long long o) {
    addTime();
    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(long o) {
    addTime();
    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(short o) {
    addTime();

    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(unsigned short o) {
    addTime();

    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(unsigned int o) {
    addTime();

    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(char o) {
    addTime();

    buf += o;
    return *this;
  }

  logger &operator<<(unsigned char o) {
    addTime();

    buf += std::to_string(o);
    return *this;
  }

  logger &operator<<(char *o) {
    addTime();

    buf += std::string(o);
    return *this;
  }

  logger &operator<<(bool o) {
    addTime();

    buf += o ? "true" : "false";
    return *this;
  }

  // logger& operator << (std::string &o) {
  //  buf += o;
  //  return *this;
  //}

  logger &operator<<(std::string o) {
    addTime();

    buf += o;
    return *this;
  }

  logger &operator<<(std::uint64_t o) {
    addTime();

    buf += std::to_string(o);
    return *this;
  }

  // logger& operator << (std::uint32_t o) {
  //  buf += std::to_string(o);
  //  return *this;
  //}

  // logger& operator << (std::uint16_t o) {
  //  buf += std::to_string(o);
  //  return *this;
  //}

  // logger& operator << (std::uint8_t o) {
  //  buf += std::to_string(o);
  //  return *this;
  //}

  // logger& operator << (std::int8_t o) {
  //  buf += std::to_string(o);
  //  return *this;
  //}

  // logger& operator << (std::int16_t o) {
  //  buf += std::to_string(o);
  //  return *this;
  //}

  // logger& operator << (std::int32_t o) {
  //  buf += std::to_string(o);
  //  return *this;
  //}

  // logger& operator << (std::int64_t o) {
  //  buf += std::to_string(o);
  //  return *this;
  //}

  // logger& operator << (std::uint32_t o) {
  //  buf += std::to_string(o);
  //  return *this;
  //}

  logger &operator<<(std::thread::id o) {
    addTime();

    std::stringbuf b;
    std::ostream os(&b);
    os << o;

    buf += b.str();
    return *this;
  }

private:
  logger(const logger &) = delete;
  logger &operator=(const logger &) = delete;
  void addTime() {
    if (!buf.empty())
      return;

    char tmp[24] = {0};
    auto now = std::chrono::system_clock::now();
    auto s =
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
            .count();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
                  now.time_since_epoch())
                  .count();
    auto left_ms = (ms - s * 1000);
    while (left_ms > 1000) {
      left_ms /= 10;
    }

    auto t = std::chrono::system_clock::to_time_t(now);
    auto tm = std::localtime(&t);
    std::strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S ", tm);
    buf += std::string(tmp);
    buf += std::to_string(left_ms) + " ";
  }
};

#endif
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
#include <cmath>

int log_init(std::string file);
void log(std::string &str);
int getBitSum(std::uint64_t num);

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

  logger &operator<<(const char *o) {
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
    std::uint64_t s =
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
            .count();
    std::uint64_t ms = std::chrono::duration_cast<std::chrono::microseconds>(
                           now.time_since_epoch())
                           .count();
    int bits_diff = getBitSum(ms) - getBitSum(s);
    std::uint64_t ttt = s * std::pow(10, bits_diff);
    std::uint64_t left_ms = ms - ttt;
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
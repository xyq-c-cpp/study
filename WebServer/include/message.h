/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of request message class.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#ifndef  _MESSAGE_H_
#define  _MESSAGE_H_

#include <unordered_map>
#include <string>

#include <../Common/Common.h>

#define WEB_SVR_IO_DEFAULT_RETRY_TIME   200

#define HTTP_WAY_GET_STR                "GET"
#define HTTP_WAY_POST_STR               "POST"
#define HTTP_WAY_HEAD_STR               "HEAD"

typedef enum {
  HTTP_WAY_GET = 0,
  HTTP_WAY_POST,
  HTTP_WAY_HEAD,
  HTTP_WAY_NONE
}http_way_t;

typedef enum {
  HTTP_VER_1_0 = 0,
  HTTP_VER_2_0,
  HTTP_VER_NONE
}http_ver_t;

class Message
{
 public:
  Message(int fd, uint8_t retry_times = WEB_SVR_IO_DEFAULT_RETRY_TIME);
  ~Message();

 private:
  static void Handle(int fd, Message *req);

  int ParseLine(void);
  int ParseHeader(void);
  int AnalyseMsg(void);
  bool HandleMsg(void);
  bool ProcMessage(uint32_t epoll_fd);
  int Fd(void);

  std::string src_msg_;
  std::unordered_map<std::string, std::string> header_;
  std::string body_;

  http_way_t way_;
  uint8_t path_[WEB_SVR_BUFF_SIZE_64];
  http_ver_t ver_;
  
  uint32_t pos_;
  int fd_;

  const uint8_t retry_times_;
  uint8_t retryed_;
};

#endif /* _MESSAGE_H_ */


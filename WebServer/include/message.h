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

#include <common.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <channal.h>

#define HTTP_WAY_GET_STR    "GET"
#define HTTP_WAY_POST_STR   "POST"
#define HTTP_WAY_HEAD_STR   "HEAD"

typedef enum {
  HTTP_WAY_GET = 0,
  HTTP_WAY_POST,
  HTTP_WAY_HEAD,
  HTTP_WAY_NONE
}http_way_t;

typedef enum {
  HTTP_VER_1_0 = 0,
  HTTP_VER_1_1,
  HTTP_VER_2_0,
  HTTP_VER_NONE
}http_ver_t;

class Message : public std::enable_shared_from_this<Message> {
 public:
  explicit Message(std::string src_mag);
  Message(const Message &another);
  Message& operator = (const Message &another);
  Message(Message &&another);
  ~Message() = default;
  
  int ProcMessage(std::shared_ptr<Channal> channal);

  void Reset();

 private:
  int ParseLine(void);
  int ParseHeader(void);
  int AnalyseMsg(void);
  int MessageRsp(std::shared_ptr<Channal> channal);

  std::string src_msg_;
  http_ver_t ver_;
  unsigned int pos_;
  http_way_t way_;
  std::string path_;

  std::unordered_map<std::string, std::string> header_;
  std::string body_;
};

#endif /* _MESSAGE_H_ */


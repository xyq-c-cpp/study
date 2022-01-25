/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of request message class.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <Buffer.h>
#include <Channal.h>
#include <Common.h>

#ifndef _MSC_VER
#include <sys/mman.h>
#include <sys/stat.h>
#endif // !_MSC_VER

#define HTTP_WAY_GET_STR "GET"
#define HTTP_WAY_POST_STR "POST"
#define HTTP_WAY_HEAD_STR "HEAD"

typedef enum {
  HTTP_WAY_GET = 0,
  HTTP_WAY_POST,
  HTTP_WAY_HEAD,
  HTTP_WAY_NONE
} http_way_t;

typedef enum {
  HTTP_VER_1_0 = 0,
  HTTP_VER_1_1,
  HTTP_VER_2_0,
  HTTP_VER_NONE
} http_ver_t;

class Message {
public:
  explicit Message(std::shared_ptr<Channal> holder);
  ~Message();

  int handleReadEvent();
  int handleWriteEvent();
  int handleErrorEvnet();
  int HandleConnectEvent();

private:
  int ProcMessage(int fd);
  void Reset();
  int ParseLine(void);
  int ParseHeader(void);
  int AnalyseMsg(void);
  int MessageRsp(int fd, bool &isClose);
  void handleErrorRsp(int fd);
  int handleGetRequest(int fd, bool &isClose);
  int handlePostRequest(int fd);

  http_ver_t ver_;
  unsigned int pos_;
  http_way_t way_;
  int retry_time_;
  std::weak_ptr<Channal> holder_;
  std::string path_;
  // 1024´æÔÚÒþ»¼
  Buffer<2048> src_msg_;
  std::string body_;
  std::map<std::string, std::string> header_;
  std::string rsp_;
  bool keep_alive;
  bool error;
};

#endif /* _MESSAGE_H_ */

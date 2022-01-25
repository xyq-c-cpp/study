/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: common function
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

// 2022.01.24
//遇到一个问题：
//当http响应头和响应体合在一起发送时，浏览器端会一直顺时针转圈；
//当http响应头和响应体分开发送时，表现就正常；
//目前，还没有找出原因。

#include <Epoller.h>
#include <Filetype.h>
#include <Message.h>
#include <Server.h>

#define INDEX_HTML_FILE_PATH "../Resource/index.html"
#define WORK_DIR "/WebServer/resource"

static const char *ver_int2str[] = {"HTTP/1.0", "HTTP/1.1", "HTTP/2.0", "NONE"};

static const char *way_int2str[] = {"GET", "POST", "HEAD", "NONE"};

std::unordered_map<std::string, std::string> fileType;

static http_way_t http_way_str2enum(std::string way_str) {
  if (!strcmp(way_str.c_str(), HTTP_WAY_GET_STR)) {
    return HTTP_WAY_GET;
  } else if (!strcmp(way_str.c_str(), HTTP_WAY_POST_STR)) {
    return HTTP_WAY_POST;
  } else if (!strcmp(way_str.c_str(), HTTP_WAY_HEAD_STR)) {
    return HTTP_WAY_HEAD;
  } else {
    return HTTP_WAY_NONE;
  }
}

static http_ver_t http_ver_str2enum(std::string ver) {
  if (!strcmp(ver.c_str(), (ver_int2str[0]))) {
    return HTTP_VER_1_0;
  } else if (!strcmp(ver.c_str(), (ver_int2str[1]))) {
    return HTTP_VER_1_1;
  } else if (!strcmp(ver.c_str(), ver_int2str[2])) {
    return HTTP_VER_2_0;
  } else {
    return HTTP_VER_NONE;
  }
}

Message::Message(std::shared_ptr<Channal> holder)
    : ver_(HTTP_VER_1_0), pos_(0), way_(HTTP_WAY_NONE), retry_time_(0),
      holder_(holder) {
  keep_alive = false;
  error = false;
  rsp_.clear();
}

Message::~Message() { Reset(); }

void Message::Reset(void) {
  // if (!src_msg_.empty()) {
  //  src_msg_.clear();
  //}

  ver_ = HTTP_VER_1_0;
  pos_ = 0;

  if (!header_.empty()) {
    header_.clear();
  }

  if (!body_.empty()) {
    body_.clear();
  }

  way_ = HTTP_WAY_NONE;
}

int Message::ParseLine(void) {
  unsigned int cur_pos = pos_, tmp_pos1;

  tmp_pos1 = src_msg_.find(' ', cur_pos);
  if (tmp_pos1 == src_msg_.end())
    return -1;

  way_ = http_way_str2enum(src_msg_.substr(cur_pos, tmp_pos1 - cur_pos));
  cur_pos = tmp_pos1 + 1;

  tmp_pos1 = src_msg_.find(' ', cur_pos);
  if (tmp_pos1 == src_msg_.end())
    return -1;

  path_ = src_msg_.substr(cur_pos, tmp_pos1 - cur_pos);
  cur_pos = tmp_pos1 + 1;

  tmp_pos1 = src_msg_.find('\r', cur_pos);
  if (tmp_pos1 == src_msg_.end())
    return -1;

  ver_ = http_ver_str2enum(src_msg_.substr(cur_pos, tmp_pos1 - cur_pos));
  pos_ = tmp_pos1 + 2;

  return 0;
}

int Message::ParseHeader() {
  unsigned int tmp1, tmp2;
  std::string key, val;

  while (true) {
    tmp1 = src_msg_.find('\r', pos_);
    if (tmp1 != src_msg_.end()) {
      /* the http header is ending */
      if (tmp1 == pos_) {
        pos_ += 2;
#if defined(DEBUG) && defined(TEST)
        for (auto &i : header_) {
          logger() << i.first << " ---> " << i.second;
        }
#endif
        return 0;
      }

      tmp2 = src_msg_.find(':', pos_);
      if (tmp2 == src_msg_.end()) {
#ifdef DEBUG
        logger() << "No expected :, not find";
#endif
        return -1;
      }

      key = src_msg_.substr(pos_, tmp2 - pos_);
      //多一个空格 tmp2 + 2 越过空格 Connection: keep-alive\r\n
      val = src_msg_.substr(tmp2 + 2, tmp1 - tmp2 - 2);
      header_.insert(std::make_pair(key, val));
      pos_ = tmp1 + 2;
    } else {
      return -1;
    }
  }
  return -1;
}

int Message::ProcMessage(int fd) {
  int ret;
  bool is_close = false;

  ret = AnalyseMsg();
  if (ret) {
#ifdef DEBUG
    logger() << "AnalyseMsg failed, ret " << ret << " fd " << fd;
#endif
    handleErrorRsp(holder_.lock()->getFd());
    return -1;
  }

  ret = MessageRsp(fd, is_close);
  if (ret != 0) {
#ifdef DEBUG
    logger() << "MessageRsp failed, ret " << ret << " fd " << fd;
#endif
    return -1;
  }

  return 0;
}

int Message::MessageRsp(int fd, bool &isClose) {
  switch (way_) {
  case http_way_t::HTTP_WAY_GET:
    return handleGetRequest(fd, isClose);
  case http_way_t::HTTP_WAY_POST:
    return handlePostRequest(fd);
  default:
    handleErrorRsp(fd);
#ifdef DEBUG
    logger() << "No support way, fd " << fd;
#endif
    return -1;
  }
}

int Message::handlePostRequest(int fd) {
  logger() << "fd " << fd << " get post request, body:\n" << body_;

  //直接返回失败
  handleErrorRsp(fd);
}

int Message::handleGetRequest(int fd, bool &isClose) {
#define TIMEOUT_TIME 5
  std::string fileType;
  struct stat st;
  int ret, resourcFd;
  char *tmp;
  const char *path = INDEX_HTML_FILE_PATH;
  std::string buff;
  static const char *shortRsp =
      "HTTP/1.1 200 OK\r\nContent-type: text/plain"
      "\r\nContent-Length: 11\r\nConnection: close\r\n\r\nHello World";
  static const char *longRsp =
      "HTTP/1.1 200 OK\r\nContent-type: text/plain"
      "\r\nContent-Length: 11\r\nConnection: keep-alive\r\nKeep-Alive: "
      "timeout=300\r\n\r\nHello World";

  // webbench压力测试
#ifdef TEST
  int len;
  if (header_.find("Connection") != header_.end() &&
      (header_["Connection"] == "Keep-Alive" ||
       header_["Connection"] == "keep-alive")) {
    isClose = false;
    keep_alive = true;
    tmp = longRsp;
    len = strlen(longRsp);
  } else {
#ifdef DEBUG
    logger() << "fd " << fd << " the HTTP header not keep alive";
#endif
    tmp = shortRsp;
    len = strlen(shortRsp);
    isClose = true;
    keep_alive = false;
  }
  ret = web_svr_write(fd, tmp, len);
  if (ret != len)
    return -1;
#else
  buff = "HTTP/1.1 200 OK\r\n";
  if (header_.find("Connection") != header_.end() &&
      !strcmp(header_["Connection"].c_str(), "keep-alive")) {
    buff += "Connection: keep-alive\r\n";
    buff +=
        "Keep-Alive: timeout=" + std::to_string(KEEP_ALIVE_TIMEOUT_S) + "\r\n";
    isClose = false;
    keep_alive = true;
  } else {
    buff += "Connection: close\r\n";
    isClose = true;
    keep_alive = false;
  }

  std::string absolutePath = std::string(WORK_DIR "/index.html");
  auto iter = path_.rfind('.');
  if (iter == std::string::npos) {
    fileType = FileType::GetFileType("default").c_str();
  } else {
    absolutePath = std::string(WORK_DIR) + path_;
    fileType = FileType::GetFileType(path_.substr(iter).c_str());
  }

  if (stat(absolutePath.c_str(), &st) < 0) {
#ifdef DEBUG
    logger() << "stat failed err " << strerror(errno);
#endif
    handleErrorRsp(fd);
    return -1;
  }

  buff += "Content-type: " + fileType + "\r\n";
  buff += "Content-length: " + std::to_string(st.st_size) + "\r\n";
  buff += "\r\n";

  auto send_ret = send(fd, buff.c_str(), buff.size(), 0);
  if (send_ret != buff.size()) {
    logger() << "send header not all, send_ret " << send_ret << " all "
             << buff.size();
  } else {
    logger() << "rsp header:\n" << buff;
  }

  // logger() << "rsp header:\n" << buff;
  // rsp_ += buff;

  //  ret = web_svr_write(fd, const_cast<char *>(buff.c_str()), buff.length());
  //  if (ret != (int)buff.length()) {
  //#ifdef DEBUG
  //    logger() << "write failed, ret " << ret << " errstr " << strerror(errno)
  //              ;
  //#endif
  //    return -1;
  //  }

  resourcFd = open(absolutePath.c_str(), O_RDONLY, 0);
  if (resourcFd < 0) {
#ifdef DEBUG
    logger() << "open failed, errstr " << strerror(errno);
#endif
    return -1;
  }

  tmp = static_cast<char *>(
      mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, resourcFd, 0));
  (void)close(resourcFd);

  rsp_ += std::string(tmp, st.st_size);

  // logger() << "fd " << fd << " rsp\n " << rsp_;

  // ret = web_svr_write(fd, tmp, st.st_size);
  ret = send(fd, tmp, st.st_size, 0);
  if (ret != st.st_size) {
#ifdef DEBUG
    logger() << "write file " << absolutePath << " not all, errstr "
             << strerror(errno) << " all " << st.st_size << " ret " << ret;
#endif
    rsp_ = rsp_.substr(ret);
    auto &t = holder_.lock()->getEvent();
    t = EPOLLOUT | EPOLLET;
  } else {
    rsp_.clear();
  }
  (void)munmap(tmp, st.st_size);
#endif
  return 0;
}

void Message::handleErrorRsp(int fd) {
  std::string body =
      "<html><title>Not Found</title><body bgcolor=\"ffffff\">Sorry, "
      "the server can not find the resource.<hr><em> XieYongQi"
      "'s WebServer CentOs 8.0 Aliyun</em>\n</body></html>";

  std::string header = "HTTP/1.1 404 Not Found!\r\n"
                       "Content-type: text/html\r\n"
                       "Connection: close\r\n"
                       "Server: XieYongQi's WebServer CentOs 8.0 Aliyun\r\n"
                       "Content-Length: " +
                       std::to_string(body.length()) + "\r\n\r\n";
  (void)web_svr_write(fd, const_cast<char *>(header.c_str()), header.length());
  (void)web_svr_write(fd, const_cast<char *>(body.c_str()), body.length());
}

int Message::AnalyseMsg() {
  int ret;

  ret = ParseLine();
  if (ret)
    return -1;

  ret = ParseHeader();
  if (ret)
    return -1;

  if (way_ == http_way_t::HTTP_WAY_GET) {

  } else if (way_ == http_way_t::HTTP_WAY_POST) {
    if (!header_["Content-length"].empty()) {
      int len = atoi(header_["Content-length"].c_str());
      body_ = src_msg_.substr(pos_, len);
      pos_ += len;
    }
  }

  src_msg_.reset(pos_);

  return 0;
}

int Message::handleReadEvent() {
  src_msg_.clear();
  std::shared_ptr<Channal> holder = holder_.lock();

  auto &event_ = holder->getEvent();

  logger() << "handle read event, fd " << holder->getFd();

  int fd = holder->getFd();
  bool rs = true;
  char buf[512];
  const int times = 3;
  int time = 0;
  do {
    memset(buf, 0, sizeof(buf));
    int read_num = recv(fd, buf, sizeof(buf), 0);
    if (read_num < 0) {
      logger() << "read_num " << read_num << " fd " << fd << " errno " << errno
               << ", " << strerror(errno);
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        event_ = EPOLLIN | EPOLLET;
        keep_alive = true;
        break;
      } else if (errno == EPIPE) {
        event_ = 0;
        keep_alive = false;
        error = true;
        return -1;
      } else {
        logger() << "can not read msg, ret " << read_num << " errno " << errno
                 << ", " << strerror(errno);
        handleErrorRsp(fd);
        error = true;
        return -1;
      }
    } else if (read_num == 0) {
      //对端已关闭
      logger() << "the peer connect close, fd " << fd << " errno" << errno
               << ", " << strerror(errno);
      event_ = 0;
      keep_alive = false;
      return -1;
    } else {
      src_msg_.append(buf, read_num);
    }
  } while (rs);

  //  int num =
  //      web_svr_read(fd, const_cast<char *>(src_msg_.c_str()),
  //      src_msg_.cap());
  //  if (num <= 0) {
  //#ifndef DEBUG
  //    logger() << "fd " << fd << " read message failed, ret " << num
  //              << " errstr " << strerror(errno) ;
  //#endif
  //    ++retry_time_;
  //    if (errno == EPIPE) {
  //      error = true;
  //      event_ = 0;
  //      return -1;
  //    } else if (retry_time_ >= MAX_RETRY_TIME){
  //      event_ = EPOLLET | EPOLLOUT;
  //      keep_alive = true;
  //    } else {
  //      event_ = (EPOLLIN | EPOLLET | EPOLLONESHOT);
  //      keep_alive = true;
  //    }
  //    return -1;
  //  }

#ifdef DEBUG
  logger() << "fd " << fd << " http request: \n" << src_msg_.c_str();
#endif

  ProcMessage(fd);

  Reset();

  return 0;
}

int Message::handleWriteEvent() {
  std::shared_ptr<Channal> holder = holder_.lock();
  if (holder == nullptr)
    return -1;

  logger() << "handle write event, fd " << holder->getFd();

  auto &event_ = holder->getEvent();
  if (rsp_.empty()) {
    event_ = EPOLLIN | EPOLLET;
  } else {
    auto ret = send(holder->getFd(), rsp_.c_str(), rsp_.size(), 0);
    if (ret < rsp_.size()) {
      event_ = EPOLLOUT | EPOLLET;
      rsp_ = rsp_.substr(ret);
    } else {
      rsp_.clear();
      event_ = EPOLLIN | EPOLLET;
    }
  }

  return 0;
}

int Message::handleErrorEvnet() {
  auto holder = holder_.lock();
  auto &event_ = holder->getEvent();

  logger() << "handle error event, fd " << holder->getFd();

  event_ = 0;
  keep_alive = false;

  return 0;
}

int Message::HandleConnectEvent() {
  auto holder = holder_.lock();
  auto &event_ = holder->getEvent();

  logger() << "handle connect, fd " << holder->getFd() << " error " << error
           << " keep_alive " << keep_alive << " event_ " << event_;

  auto epoller = holder->getEpoller();

  holder->delTimer();

  if (!error) {
    if (event_ != 0) {
      epoller->epollMod(holder->getFd(), event_, KEEP_ALIVE_TIMEOUT_MS);
    } else if (keep_alive) {
      epoller->epollMod(holder->getFd(), EPOLLIN | EPOLLET,
                        KEEP_ALIVE_TIMEOUT_MS);
    } else {
      epoller->epollDel(holder->getFd());
    }
  } else {
    epoller->epollDel(holder->getFd());
  }

  return 0;
}

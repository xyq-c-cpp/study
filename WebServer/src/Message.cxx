/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: common function
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

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
      holder_(holder) {}

// Message::Message(const Message &another) {
//  src_msg_ = another.src_msg_;
//  ver_ = another.ver_;
//  pos_ = another.pos_;
//  way_ = another.way_;
//  header_ = another.header_;
//  body_ = another.body_;
//}
//
// Message& Message::operator = (const Message &another) {
//  src_msg_ = another.src_msg_;
//  pos_ = another.pos_;
//  ver_ = another.ver_;
//  way_ = another.way_;
//  header_ = another.header_;
//  body_ = another.body_;
//
//  return *this;
//}
//
// Message::Message(Message &&another)
//  : src_msg_(std::move(another.src_msg_)),
//    ver_(another.ver_),
//    pos_(another.pos_),
//    way_(another.way_),
//    header_(std::move(another.header_)),
//    body_(std::move(another.body_)) {
//}

Message::~Message() { Reset(); }

void Message::Reset(void) {
  if (!src_msg_.empty()) {
    src_msg_.clear();
  }

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
          std::cout << i.first << " ---> " << i.second << std::endl;
        }
#endif
        return 0;
      }

      tmp2 = src_msg_.find(':', pos_);
      if (tmp2 == src_msg_.end()) {
#ifdef DEBUG
        std::cout << "No expected :, not find" << std::endl;
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
    std::cout << "AnalyseMsg failed, ret " << ret << std::endl;
#endif
    return -1;
  }

  ret = MessageRsp(fd, is_close);
  if (ret) {
#ifdef DEBUG
    std::cout << "MessageRsp failed, ret " << ret << std::endl;
#endif
    return -2;
  }

  if (is_close)
    return -2;
  return 0;
}

int Message::MessageRsp(int fd, bool &isClose) {
  switch (way_) {
  case http_way_t::HTTP_WAY_GET:
    return handleGetRequest(fd, isClose);
  default:
    handleErrorRsp(fd);
#ifdef DEBUG
    std::cout << "No support way" << std::endl;
#endif
    return -1;
  }
}

int Message::handleGetRequest(int fd, bool &isClose) {
#define TIMEOUT_TIME 5
  std::string fileType;
  struct stat st;
  int ret, resourcFd;
  char *tmp;
  const char *path = INDEX_HTML_FILE_PATH;
  std::string buff;
  static char *shortRsp =
      "HTTP/1.1 200 OK\r\nContent-type: text/plain"
      "\r\nContent-Length: 11\r\nConnection: close\r\n\r\nHello World";
  static char *longRsp =
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
    tmp = longRsp;
    len = strlen(longRsp);
  } else {
#ifdef DEBUG
    std::cout << "fd " << fd << " the HTTP header not keep alive" << std::endl;
#endif
    tmp = shortRsp;
    len = strlen(shortRsp);
    isClose = true;
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
  } else {
    buff += "Connection: close\r\n";
    isClose = true;
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
    std::cout << "stat failed err " << strerror(errno) << std::endl;
#endif
    handleErrorRsp(fd);
    return -1;
  }

  buff += "Content-type: " + fileType + "\r\n";
  buff += "Content-length: " + std::to_string(st.st_size) + "\r\n";
  buff += "\r\n";

  ret = web_svr_write(fd, const_cast<char *>(buff.c_str()), buff.length());
  if (ret != (int)buff.length()) {
#ifdef DEBUG
    std::cout << "write failed, ret " << ret << " errstr " << strerror(errno)
              << std::endl;
#endif
    return -1;
  }

  resourcFd = open(absolutePath.c_str(), O_RDONLY, 0);
  if (resourcFd < 0) {
#ifdef DEBUG
    std::cout << "open failed, errstr " << strerror(errno) << std::endl;
#endif
    return -1;
  }

  tmp = static_cast<char *>(
      mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, resourcFd, 0));
  (void)close(resourcFd);

  ret = web_svr_write(fd, tmp, st.st_size);
  if (ret != st.st_size) {
#ifdef DEBUG
    std::cout << "write file " << absolutePath << " failed, errstr"
              << strerror(errno) << std::endl;
#endif
    (void)munmap(tmp, st.st_size);
    return -1;
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

  body_ = src_msg_.substr(pos_);
  return 0;
}

int Message::handleReadEvent() {
  src_msg_.clear();
  std::shared_ptr<Channal> holder = holder_.lock();
  if (holder == nullptr)
    return -1;

  int fd = holder->getFd();
  int num =
      web_svr_read(fd, const_cast<char *>(src_msg_.c_str()), src_msg_.cap());
  if (num <= 0) {
#ifdef DEBUG
    std::cout << "fd " << fd << " read message failed, ret " << num
              << " errstr " << strerror(errno) << std::endl;
#endif
    ++retry_time_;
    if (errno == EPIPE || retry_time_ >= MAX_RETRY_TIME) {
      holder->setErase(true);
      return -1;
    }
    holder->setEvent(EPOLLIN | EPOLLET | EPOLLONESHOT);
    holder->setIsUpdateEvent(true);
    return -1;
  }
  src_msg_.setSize(num);
#ifdef DEBUG
  std::cout << "fd " << fd << " http request: \n"
            << src_msg_.c_str() << std::endl;
#endif

  int ret = ProcMessage(fd);
  if (ret == -1) {
#ifdef DEBUG
    std::cout << "ProcMessage failed, to delete fd " << fd << std::endl;
#endif
    holder->setErase(true);
  } else if (ret == -2) {
#ifdef DEBUG
    std::cout << "short connection, to delete fd " << fd << std::endl;
#endif
    holder->setErase(true);
  } else {
#ifdef DEBUG
    std::cout << "long connection, keep alive, fd " << fd << std::endl;
#endif
    holder->setEvent(EPOLLIN | EPOLLET);
    holder->setIsUpdateEvent(true);
    holder->setErase(false);
  }
  Reset();

  return 0;
}

int Message::handleWriteEvent() { return 0; }

int Message::handleErrorEvnet() {
  auto holder = holder_.lock();
  if (holder == nullptr) {
#ifdef DEBUG
    std::cout << "holder weak_ptr to shared_ptr failed" << std::endl;
#endif
    return -1;
  }
  holder->setErase(true);
  return 0;
}

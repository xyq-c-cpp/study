/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: common function
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <message.h>
#include <filetype.h>
#include <server.h>
#include <epoller.h>

#define INDEX_HTML_FILE_PATH   "../Resource/index.html"

static const char *ver_int2str[] = {
  "HTTP/1.0",
  "HTTP/1.1",
  "HTTP/2.0",
  "NONE"
};

static const char *way_int2str[] = {
  "GET",
  "POST",
  "HEAD",
  "NONE"
};

std::unordered_map<std::string, std::string> file_type;

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
  }else {
    return HTTP_VER_NONE;
  }
}

Message::Message(std::string src_msg)
  : src_msg_(std::move(src_msg)),
    ver_(HTTP_VER_1_0), 
    pos_(0),
    way_(HTTP_WAY_NONE) {

}

Message::Message(const Message &another) {
  src_msg_ = another.src_msg_;
  ver_ = another.ver_;
  pos_ = another.pos_;
  way_ = another.way_;
  header_ = another.header_;
  body_ = another.body_;
}

Message& Message::operator = (const Message &another) {
  src_msg_ = another.src_msg_;
  ver_ = another.ver_;
  pos_ = another.pos_;
  way_ = another.way_;
  header_ = another.header_;
  body_ = another.body_;

  return *this;
}

Message::Message(Message &&another)
  : src_msg_(std::move(another.src_msg_)),
    ver_(another.ver_),
    pos_(another.pos_),
    way_(another.way_),
    header_(std::move(another.header_)),
    body_(std::move(another.body_)) {
    Reset();
}

Message::~Message() {
  LOG_DEBUG("release http message, way %s, ver %s, path %s", 
    way_int2str[way_], ver_int2str[ver_], path_.c_str());
  header_.clear();
}

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

  LOG_DEBUG("begin parse http line, src_msg size %d, pos %d", 
    src_msg_.size(), pos_);

  tmp_pos1 = src_msg_.find_first_of(" ", cur_pos);
  if (tmp_pos1 == std::string::npos) {
    LOG_ERROR("cannot find the http line ' '");
    return -1;
  }

  way_ = http_way_str2enum(src_msg_.substr(cur_pos, tmp_pos1 - cur_pos));
  cur_pos = tmp_pos1 + 1;

  tmp_pos1 = src_msg_.find_first_of(" ", cur_pos);
  if (tmp_pos1 == std::string::npos) {
    LOG_ERROR("No find the http line ' '");
    return -1;
  }

  path_ = src_msg_.substr(cur_pos, tmp_pos1 - cur_pos);
  
  cur_pos = tmp_pos1 + 1;

  tmp_pos1 = src_msg_.find_first_of("\r", cur_pos);
  if (tmp_pos1 == std::string::npos) {
    LOG_ERROR("No find htpp line end");
    return -1;
  }

  ver_ = http_ver_str2enum(src_msg_.substr(cur_pos, tmp_pos1 - cur_pos));
  pos_ = tmp_pos1 + 2;

  return 0;
}

int Message::ParseHeader() {
  unsigned int tmp1, tmp2;
  std::string key, val;

  LOG_DEBUG("begin parse http header, pos %d", pos_);

  while (true) {
    tmp1 = src_msg_.find_first_of("\r", pos_);
    if (tmp1 != std::string::npos) {
      /* the http header is ending */
      if (tmp1 == pos_) {
        pos_ += 2;
        return 0;
      }

      tmp2 = src_msg_.find_first_of(":", pos_);
      if (tmp2 == std::string::npos) {
        return -1;
      }

      key = src_msg_.substr(pos_, tmp2 - pos_);
      val = src_msg_.substr(tmp2 + 1, tmp1 - tmp2 - 1);
      header_.insert(std::make_pair(key, val));
      pos_ = tmp1 + 2;
    } else {
      return -1;
    }
  }

  return -1;
}

int Message::ProcMessage(std::shared_ptr<Channal> channal) {
  int ret;
  bool is_close = false;

  LOG_DEBUG("get http message:\n%s", src_msg_.c_str());
  
  ret = AnalyseMsg();
  if (ret) {
    LOG_ERROR("analyse http message failed, ret %d", ret);
    channal->GetServer()->GetEpoller()->DelFd(channal->Fd());
    return -1;
  }

  ret = MessageRsp(channal, is_close);
  if (ret) {
    LOG_ERROR("the message rsp failed, ret %d", ret);
    channal->GetServer()->GetEpoller()->DelFd(channal->Fd());
    return -1;
  }
  
  if (is_close) {
      channal->GetServer()->GetEpoller()->DelFd(channal->Fd());
  }

  return 0;
}

int Message::MessageRsp(std::shared_ptr<Channal> channal, bool &is_close) {
  char buff[WEB_SVR_BUFF_SIZE_2048] = {0};
  #define TIMEOUT_TIME 5
  std::string file_type;
  struct stat st;
  int ret, fd;
  char *tmp;
  const char *path = INDEX_HTML_FILE_PATH;

  LOG_DEBUG("construct http respone...");

  sprintf(buff, "%s %d %s\r\n", ver_int2str[ver_], 200, "OK");

  if (header_.find("Connection") != header_.end() &&
      !strcmp(header_["Connection"].c_str(), "keep-alive")) {
    sprintf(buff, "%sConnection: keep-alive\r\n", buff);
    sprintf(buff, "%sKeep-Alive: timeout=%d\r\n", buff, TIMEOUT_TIME);
    is_close = false;
  } else {
    sprintf(buff, "%sConnection: close\r\n", buff);
    is_close = true;
  }

  file_type = FileType::GetFileType("default").c_str();

  if (stat(path, &st) < 0) {
    LOG_ERROR("Index html file %s not exist, exit", path);
    return -1;
  }

  sprintf(buff, "%sContent-type: %s\r\n", buff, file_type.c_str());
  sprintf(buff, "%sContent-length: %d\r\n", buff, static_cast<int>(st.st_size));
  sprintf(buff, "%s\r\n", buff);
  
  LOG_DEBUG("rsp header: \n%s", buff);

  ret = channal->WriteRsp(buff, strlen(buff));
  if (ret) {
    LOG_ERROR("write rsp header failed, ret %d", ret);
    return -1;
  }

  fd = open(path, O_RDONLY, 0);
  if (fd < 0) {
    LOG_ERROR("open resource file %s failed, errno %d", path, errno);
    return -1;
  }

  tmp = static_cast<char *>(mmap(NULL, st.st_size, PROT_READ,
    MAP_PRIVATE, fd, 0));
  close(fd);

  ret = channal->WriteRsp(tmp, st.st_size);
  if (ret) {
    LOG_ERROR("write rsp body failed, ret %d", ret);
    munmap(tmp, st.st_size);
    return -1;
  }

  munmap(tmp, st.st_size);

  return 0;
}

int Message::AnalyseMsg() {
  int ret;
  
  ret = ParseLine();
  if (ret) {
    LOG_ERROR("parse http line failed, ret %d", ret);
    return -1;
  }
  
  ret = ParseHeader();
  if (ret) {
    LOG_ERROR("parse http header failed, ret %d", ret);
    return -1;
  }

  body_ = src_msg_.substr(pos_);

  LOG_DEBUG("prase http message finish, http ver %s, way %s, path_ %s\n", \
    ver_int2str[ver_], way_int2str[way_], path_.c_str());

  for (auto& i : header_) {
    LOG_DEBUG("http header, key %s, val %s", i.first.c_str(), 
      i.second.c_str());
  }

  return 0;
}


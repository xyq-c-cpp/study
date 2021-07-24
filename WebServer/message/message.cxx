/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: common function
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <signal.h>

#include <Message.h>
#include <common.h>
#include <filetype.h>

static const uint8_t *ver_int2str[] = {
  "HTTP/1.0",
  "HTTP/1.1"
  "HTTP/2.0"
  "NONE"
};

#define WHITE_SPACE_DOT   "."

std::unordered_map<std::string, std::string> file_type;

static http_way_t http_way_str2enum(std::string &way_str) {
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

static http_ver_t http_ver_str2enum(std::string &ver) {
  if (!strcmp(ver.c_str(), ver_int2str[0])) {
    return HTTP_VER_1_0;
  } else if (!strcmp(ver.c_str(), ver_int2str[1])) {
    return HTTP_VER_2_0;
  } else {
    return HTTP_VER_NONE;
  }
}

Message::Message(int fd, std::string &msg)
  : fd_(fd), 
    src_msg_(std::move(msg)), 
    ver_(HTTP_VER_1_0), 
    pos_(0),
    way_(HTTP_WAY_NONE){
  memset(path, 0, sizeof(path_));
}

Message::Message(Message &&another)
  : fd_(another.fd_), 
    src_msg_(std::move(another.src_msg_)),
    ver_(another.ver_),
    pos_(another.pos_),
    header_(std::move(another.header_)),
    body_(std::move(another.body_)),
    way_(another.way_) {
    memcpy(path_, another.path_, sizeof(path_));
    Reset();
}

Message::Reset(void) {
  fd_ = -1;
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
  memset(path_, 0, sizeof(path_));
}


int Message::ParseLine(void) {
  uint32_t pre_pos = 0, cur_pos = pos_, tmp_pos1, tmp_pos2;

  tmp_pos1 = src_msg_.find_first_of(' ', cur_pos);
  if (tmp_pos1 == std::string::npos) {
    return -1;
  }
  way_ = http_way_str2enum(src_msg_.substr(0, tmp_pos1));
  pre_pos = cur_pos;
  cur_pos -= tmp_pos1 + 1;

  tmp_pos1 = src_msg_.find_first_of('?', cur_pos);
  if (tmp_pos1 == std::string::npos) {
    return -1;
  }

  tmp_pos2 = src_msg_.find_first_of(' ', tmp_pos1);
  if (tmp_pos2 == std::string::npos) {
    return -1;
  }
  strncpy(path_, src_msg_.substr(tmp_pos1 + 1, tmp_pos2 - tmp_pos1 - 1).c_str(),
    sizeof(path_));
  pre_pos = cur_pos;
  cur_pos = tmp_pos2 + 1;

  tmp_pos1 = src_msg_.find_first_of('\r', cur_pos);
  if (tmp_pos1 == std::string::npos) {
    return -1;
  }
  ver_ = http_ver_str2enum(src_msg_.substr(cur_pos + 1, tmp_pos1 - cur_pos - 1));
  pos_ = tmp_pos1 + 2;

  return 0;
}

void Message::ParseHeader()
{
  uint32_t tmp1, tmp2;
  std::string key, val;

  while (true) {
    tmp1 = src_msg_.find_first_of('\r', pos_);
    if (tmp1 != std::string::npos) {
      tmp2 = src_msg_.find(':', tmp1);
      if (tmp2 == std::string::npos) {
        if (tmp1 == tmp2) {
          break;
        }
      }

      key = src_msg_.substr(Pos, _npos - Pos);
      val = src_msg_.substr(_npos + 1, _Pos - _npos - 1);
      header_.insert(std::pair<std::string, std::string>(key, key));
      pos_ = tmp2 + 2;
    } else {
      break;
    }
  }
}

bool Message::ProcMessage(uint32_t epoll_fd) {
  uint8_t buf[WEB_SVR_BUFF_SIZE_4096] = {0};
  int read_num = 0;
  bool is_error = false;

  while (true) {
    read_num = web_svr_read(fd_, buf, sizeof(buf) - 1);
    if (read_num < 0) {
      is_error = true;
      break;
    } else if (read_num == 0) {
      if (errno == EAGAIN) {
        if (retryed_ > retry_times_) {
          is_error = true;
          break;
        } else {
          ++retryed_;
        }
      } else if (errno != 0) {
        is_error = true;
      } else {
        break;
      }
    }

    src_msg_ += std::string(buf, buf + read_num);
  }

  if (is_error) {
    return false;
  }

  if(src_msg_.empty()) {
    return false;
  }

  print("the message had receced is : ", src_msg_, "  next , system will analyse "
    "message ....... fd   :   ", fd_);

  analyseMsg();

  if (!handleMsg()) {
    print("handlemsg fail ...... fd   :   ", Fd);
    return false;
  }

  return true;
}

bool Message::HandleMsg() {
  uint8_t sendbuffer[WEB_SVR_BUFF_SIZE_4096] = {0};

  sprintf(sendbuffer, "%s %d %s\r\n", HttpVersion.c_str(),200, "OK");

  if (RequestHeader.find("Connection") != RequestHeader.end() &&
      RequestHeader["Connection"] == "keep-alive") {
    sprintf(sendbuffer, "%sConnection: keep-alive\r\n", sendbuffer);
    sprintf(sendbuffer, "%sKeep-Alive: timeout=%d\r\n", sendbuffer, MAX_TIME_OUT);
  }

  std::string filepath = FILE_UNNDER_WORK + SourcePath;

  const char* _filetype = nullptr;
  int pos = filepath.find_last_of('.');
  if (pos == std::string::npos)  {
    _filetype = filetype::gettype("default").c_str();
  } else {
    _filetype = filetype::gettype(filepath.substr(pos)).c_str();
  }

  struct stat sfind;
  if (stat(filepath.c_str(), &sfind) < 0) {
    print("error can not find the file ,the file name is  :   ", filepath);
    return false;
  }

  sprintf(sendbuffer, "%sContent-type: %s\r\n", sendbuffer, _filetype);
  sprintf(sendbuffer, "%sContent-length: %d\r\n", sendbuffer, sfind.st_size);
  sprintf(sendbuffer, "%s\r\n", sendbuffer);
  
  print("the message header  :  ", sendbuffer);

  ssize_t nowrite = _write(Fd, sendbuffer, strlen(sendbuffer));

  if (nowrite != strlen(sendbuffer) || nowrite == 0 || nowrite == -1) {
    print("error : send message header fail.....");
    return false;
  }

  print("the size of message header had writen is :",nowrite);

  int o_fd = open(filepath.c_str(), O_RDONLY, 0);
  char *cfile = static_cast<char*>(mmap(NULL, sfind.st_size, PROT_READ,
    MAP_PRIVATE, o_fd, 0));
  close(o_fd);

  ssize_t wlen = _write(Fd, cfile, sfind.st_size);
  if (wlen != sfind.st_size || wlen == 0 || nowrite == -1) {
    print("error: send message body fail.......");
    return false;
  }

  print("send message body success,size : ",wlen);
  munmap(cfile, sfind.st_size);

  return true;
}

void Message::AnalyseMsg() {
  parseline();
  parseheader();
}

int Message::Fd(void) {
  return Fd;
}

void Message::Handle(int _fd, Message* _req) {
  _req->handlequest(_fd);
}


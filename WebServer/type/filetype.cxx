/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: define the type of file
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <filetype.h>

/* init static valiable outside class */
std::unordered_map<std::string ,std::string> filetype::mime__;
std::mutex filetype::lock_;

std::string filetype::gettype(const std::string &_type) {
  if (mime_.size() == 0) {
    std::lock_guard<std::mutex> lock(lock_);
    { 
      mime_[".html"] = "text/html";
      mime_[".avi"] = "video/x-msvideo";
      mime_[".bmp"] = "image/bmp";
      mime_[".c"] = "text/plain";
      mime_[".doc"] = "application/msword";
      mime_[".gif"] = "image/gif";
      mime_[".gz"] = "application/x-gzip";
      mime_[".htm"] = "text/html";
      mime_[".ico"] = "application/x-ico";
      mime_[".jpg"] = "image/jpeg";
      mime_[".png"] = "image/png";
      mime_[".txt"] = "text/plain";
      mime_[".mp3"] = "audio/mp3";
      mime_["default"] = "text/html";
    }
  }
  if (mime_.find(_type) == mime_.end()) {
    return mime_["default"];
  } else {
    return mime_[_type];
  }
}


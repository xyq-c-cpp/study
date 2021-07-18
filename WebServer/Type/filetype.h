/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: define the type of file
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#ifndef _FILETYPE_H_
#define _FILETYPE_H_

#include <unordered_map>
#include <string>
#include <pthread.h>
#include <mutex>

/*
 * the single class of getting type of files.
 */
class filetype
{
 public:
  filetype(const filetype& another) = delete;
  filetype& operator =(const filetype& another) = delete;
  static std::string gettype(const std::string &_type);

 private:
  filetype(){}
  
  static std::unordered_map<std::string ,std::string> mime_;
  static std::mutex lock_;
};

#endif

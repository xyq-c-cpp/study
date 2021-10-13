/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: define the type of file
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#ifndef _FILE_TYPE_H_
#define _FILE_TYPE_H_

#include <common.h>

/*
 * the single class of getting type of files.
 */
class FileType {
 public:
  FileType(const FileType& another) = delete;
  FileType& operator =(const FileType& another) = delete;

  static std::string GetFileType(const std::string &type);

 private:
  FileType() = default;
  ~FileType() = default;
  
  static std::unordered_map<std::string ,std::string> mime_;
  static std::mutex lock_;
};

#endif /* _FILE_TYPE_H_ */


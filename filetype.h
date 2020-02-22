#ifndef _FILETYPE_H_
#define _FILETYPE_H_

#include <unordered_map>
#include <string>
#include <pthread.h>
#include <mutex>

class filetype
{
	private:
		static std::unordered_map<std::string ,std::string> mime;
		static std::mutex lock;
		filetype(){}
	public:
		static std::string gettype(const std::string &_type);
};

#endif

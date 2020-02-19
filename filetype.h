#ifndef _FILETYPE_H_
#define _FILETYPE_H_

#include <unordered_map>
#include <string>
#include <pthread.h>

class filetype
{
	private:
		static std::unordered_map<std::string ,std::string> mime;
		static pthread_mutex_t lock;
		filetype(){}
	public:
		static std::string gettype(const std::string &_type);
};

#endif

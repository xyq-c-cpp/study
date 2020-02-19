#include "filetype.h"

pthread_mutex_t filetype::lock = PTHREAD_MUTEX_INITIALIZER;
std::unordered_map<std::string ,std::string> filetype::mime;

std::string filetype::gettype(const std::string &_type)
{
	if(mime.size() == 0)
	{
		pthread_mutex_lock(&lock);
		{	
			    mime[".html"] = "text/html";
			    mime[".avi"] = "video/x-msvideo";
			    mime[".bmp"] = "image/bmp";
			    mime[".c"] = "text/plain";
			    mime[".doc"] = "application/msword";
			    mime[".gif"] = "image/gif";
			    mime[".gz"] = "application/x-gzip";
			    mime[".htm"] = "text/html";
			    mime[".ico"] = "application/x-ico";
			    mime[".jpg"] = "image/jpeg";
			    mime[".png"] = "image/png";
			    mime[".txt"] = "text/plain";
			    mime[".mp3"] = "audio/mp3";
			    mime["default"] = "text/html";
	 
		}
		pthread_mutex_unlock(&lock);
	}
	if(mime.find(_type) == mime.end())
	{
		return mime["default"];
	}else 	return mime[_type];
}


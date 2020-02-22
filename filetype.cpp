#include "filetype.h"


std::unordered_map<std::string ,std::string> filetype::mime;
std::mutex filetype::lock;

std::string filetype::gettype(const std::string &_type)
{
	if(mime.size() == 0)
	{
		std::lock_guard<std::mutex> _lock(lock);
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
		
	}
	if(mime.find(_type) == mime.end())
	{
		return mime["default"];
	}else 	return mime[_type];
}


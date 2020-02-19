#include "requestmsg.h"
#include "common.h"
#include "filetype.h"

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


const char* HttpVersion_Default = { "HTTP/1.0" };
const char* HttpVersion_Other = { "HTTP/1.1" };

const int MAX_BUFFER_SIZE = 4096;
const int MAX_AGAIN_TIME = 200;
const int MAX_TIME_OUT = 500;

const std::string FILE_UNNDER_WORK = ".";

std::unordered_map<std::string,std::string> filetype;

requestmsg::requestmsg(int _Fd)
{   
    Fd = _Fd;
	HttpVersion = HttpVersion_Default;
    Pos = 0;
    MaxAgainTime = 0;
    RequestMsg = {0};
}
requestmsg::~requestmsg()
{
	if (!RequestHeader.empty())
		RequestHeader.clear();
}
void requestmsg::parseline()
{
	Pos = RequestMsg.find_first_of(' ', Pos);
	if (Pos != std::string::npos)
	{
		Methon = RequestMsg.substr(0, Pos);
	}
	size_t _Pos = RequestMsg.find_first_of('?', Pos);//get 192.168.56.709:80?/html HTTP/1.0/r/ncontengt:msg,
	if (_Pos != std::string::npos)
	{
		Pos = RequestMsg.find_first_of(' ', _Pos);
		if (Pos != std::string::npos)
		{
			SourcePath = RequestMsg.substr(_Pos + 1, Pos - _Pos - 1);
		}
	}
	_Pos = RequestMsg.find_first_of('\r', Pos+1);
	if (_Pos != std::string::npos)
	{
		HttpVersion = RequestMsg.substr(Pos+1, _Pos - Pos - 1);
	}
	else {
		HttpVersion = HttpVersion_Default;
	}
	Pos = _Pos + 2;
}
void requestmsg::parseheader()
{
	size_t _Pos, _npos;
	std::string _key, _value;
	while (1)
	{
		//GET 192.168.56.709:80?/html HTTP/1.0\r\ncontengt:msg\r\ncontent_length:17\r\n\r\n
		_Pos = RequestMsg.find_first_of('\r', Pos);
		if (_Pos != std::string::npos)
		{
			_npos = RequestMsg.find(':', Pos);
			if (_npos == std::string::npos)
			{
				if (_Pos == Pos)
				{
					break;
				}
			}
			_key = RequestMsg.substr(Pos, _npos - Pos );
			_value = RequestMsg.substr(_npos + 1, _Pos - _npos - 1 );
			RequestHeader.insert(std::pair<std::string, std::string>(_key, _value));
			Pos = _Pos + 2;

		}
		else break;
	}
}
bool requestmsg::handlequest(int _epollfd)
{
    char buffer[MAX_BUFFER_SIZE] = {0};
    int nowread = 0;
    bool iserror = false;
    while(1)
    {
        nowread = _read(Fd, buffer, MAX_BUFFER_SIZE);
        if(nowread < 0)
        {
            iserror = true;
            break;
        }else if(nowread == 0)
        {
            if(errno == EAGAIN)
            {
                if(MaxAgainTime > MAX_AGAIN_TIME)
                {
                    iserror = true;
                    break;
                }
                else ++MaxAgainTime;
            }
            else if (errno != 0)            
                iserror = true;
            break;            
        }
        std::string msg(buffer, buffer + nowread);
        RequestMsg += msg;
    }
    if(iserror == true)
    {
        close(Fd);
        delete this;
        return false;
    }
    analysemsg();
    
    if(handlemsg() == false )
    {
        print("handlemsg fail ........");
        return false;
    }
    
    __uint32_t events = EPOLLIN | EPOLLET | EPOLLONESHOT ;
    if( false == _epoll_ctl(_epollfd, EPOLL_CTL_MOD, static_cast<void*>(this), Fd, events) )
    print("epoll_ctl fail add again !........");

    return true;
}
bool requestmsg::handlemsg()
{
        char sendbuffer[MAX_BUFFER_SIZE] = {0};
		
		sprintf(sendbuffer,"%s %d %s\r\n",HttpVersion.c_str(),200,"OK");
		
		if (RequestHeader.find("Connection") != RequestHeader.end() && RequestHeader["Connection"] == "keep-alive")
		{
			sprintf(sendbuffer,"%sConnection: keep-alive\r\n",sendbuffer);
			sprintf(sendbuffer,"%sKeep-Alive: timeout=%d\r\n",sendbuffer,MAX_TIME_OUT);
		}	

		std::string filepath = FILE_UNNDER_WORK + SourcePath;
		
		const char* _filetype = nullptr;
		int pos = filepath.find_last_of('.');
		if (pos == std::string::npos) 
			_filetype = filetype::gettype("default").c_str();
		else 
			_filetype = filetype::gettype(filepath.substr(pos)).c_str();

		struct stat sfind;
		if(stat(filepath.c_str(),&sfind) < 0)
		{
			print("error can not find the file ,webserver will be exit !");
			return false;
		}
		sprintf(sendbuffer,"%sContent-type: %s\r\n",sendbuffer,_filetype);

		sprintf(sendbuffer,"%sContent-length: %d\r\n",sendbuffer,sfind.st_size);

		sprintf(sendbuffer,"%s\r\n",sendbuffer);
		
		print("the message header : ",sendbuffer);
	
		ssize_t nowrite = _write(Fd, sendbuffer, strlen(sendbuffer));

		if(nowrite != strlen(sendbuffer) || nowrite == 0 || nowrite == -1)
		{
			print("error : send message header fail");       
			return false;
		}
		print("the size of message header had writen is :",nowrite);
		
		int o_fd = open(filepath.c_str(),O_RDONLY,0);

		char * cfile = static_cast<char*>(mmap(NULL, sfind.st_size, PROT_READ, MAP_PRIVATE, o_fd, 0));

		close(o_fd);

		ssize_t wlen = _write(Fd, cfile, sfind.st_size);
		if(wlen != sfind.st_size || wlen == 0 || nowrite == -1)
		{
			print("error: send message body fail ");            
			return false;
		}
		print("send message body success,size : ",wlen);

		munmap(cfile,sfind.st_size);
        
        return true;
}
void requestmsg::analysemsg()
{
	parseline();
	parseheader();
}
int requestmsg::getfd()
{
    return Fd;
}









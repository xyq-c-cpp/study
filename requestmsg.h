#ifndef  _REQUESTMSG_H_
#define  _REQUESTMSG_H_

#include <unordered_map>
#include <string>

class requestmsg
{
private:
	std::string RequestMsg;
	std::unordered_map<std::string, std::string> RequestHeader;
	std::string RequestBody;

	std::string Methon;
	std::string SourcePath;
	std::string HttpVersion;

	size_t Pos;
    int Fd;
    int MaxAgainTime;
    
public:
	requestmsg(int _Fd);	
	~requestmsg();

private:
	void parseline();
	void parseheader();
	void analysemsg();
    
    bool handlemsg();
    
public:	

    static void handle(int _fd,requestmsg* _req);
    bool handlequest(int _epollfd);    
    int getfd();          
    
};

#endif // ! _REQUESTMSG_H_



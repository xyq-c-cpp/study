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
public:	

    int getfd();          
    bool handlequest(int _epollfd);
    bool handlemsg();
};

#endif // ! _REQUESTMSG_H_



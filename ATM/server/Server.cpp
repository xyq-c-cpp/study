#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include "winsock.h"
#include "mysql.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"libmysql.lib")

using namespace std;

struct conn_info //连接关键字
{
	char *host;
	char *user;
	char *password;
	char *db;
};

int Getmoney(MYSQL* mysql,char card[]);

MYSQL* mysql_conn_setup(conn_info con)     //连接到mysql
{
	MYSQL *mysql=mysql_init(NULL);         //API 获取或初始化MYSQL结构。
	if(!mysql_real_connect(mysql,con.host,con.user,con.password,con.db,3306,NULL,0)) //API
	{
		exit(1);
	}
	return mysql;
}

char UpdateRe(MYSQL* mysql,char card[],int money)
{
	int ret;
	char query[100];

	sprintf(query,"update user set money='%d' where card='%s';",money,card);
	ret=mysql_query(mysql,query);
	if(ret)
	{
		return '0';
		exit(1);
	}	
	else
		return '1';
}


char UpdateIn(MYSQL* mysql,char card[],int money)
{
	int ret;
	char query[100];

	sprintf(query,"update user set money='%d' where card='%s';",money,card);

	ret=mysql_query(mysql,query);
	if(ret)
	{
		return '0';
		exit(1);
	}	
	else
		return '1';
}

char IsExit(MYSQL* mysql,char card[],char password[])
{
	int ret;
	char query[100];
	MYSQL_RES *recordSet; 
	sprintf(query,"select * from user where card='%s' and password='%s';",card,password);

	ret=mysql_query(mysql,query);
	if(ret)
	{
		return '0';
	}
	else
	{
		recordSet = mysql_store_result(mysql);
		if(mysql_num_rows(recordSet)==0)
			return '0';
		else
			return '1';
	}
}

int Getmoney(MYSQL* mysql,char card[])
{
	int ret,i,j;
	char query[100];
	MYSQL_RES *recordSet; 
	MYSQL_ROW row;
	char p[3][20];       //3*20
	int rem=0;
    sprintf(query,"select money from user where card='%s';",card);

	ret=mysql_query(mysql,query);
	if(ret)
	{
		exit(1);
	}
	else
	{
		recordSet = mysql_store_result(mysql);
		row=mysql_fetch_row(recordSet);
		strcpy(p[0],row[0]);

		for(i=0;i<strlen(p[0]);i++)
		{
			j=p[0][i]-'0';
			rem=rem*10+j;

		}
		return rem;
	}
}


int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2),&wsaData))   //初始化WinSock协议栈
	{
		cout<<"Winsock不能被初始化！";
		WSACleanup();
		return 0;
	}

	SOCKET sockSer,sockConn;                //注意服务器端必须创建两个套接字
	sockSer=socket(AF_INET,SOCK_STREAM,0);  //初始化套接字
    SOCKADDR_IN addrSer,addrCli;            //注意服务器端要创建两个套接字地址     
	
	addrSer.sin_family=AF_INET;             //连接
	addrSer.sin_port=htons(5566);
	addrSer.sin_addr.S_un.S_addr=inet_addr("172.16.8.17");

	bind(sockSer,(SOCKADDR*)&addrSer,sizeof(SOCKADDR));  //绑定套接字
	listen(sockSer,5);                                   //监听
	int len=sizeof(SOCKADDR);
	cout<<"服务器等待客户端的连接......"<<endl;

	sockConn=accept(sockSer,(SOCKADDR*)&addrCli,&len);  //接受连接，注意返回值
    if(sockConn==INVALID_SOCKET)
	{
		cout<<"服务器接受客户端连接失败！"<<endl;
		return 0;
	}
	else
		cout<<"服务器接受客户端连接成功！"<<endl;


	char sendbuf[256],recvbuf[256];          //接收发送缓冲区
	char str1,str2[20],str3[20],str4[10];    //分析字段




    MYSQL *conn;                       //定义数据库
	struct conn_info info;             //数据库相关信息
	info.host="localhost";
	info.user="root";
	info.password="123";
	info.db="test";
	conn=mysql_conn_setup(info);       //连接到数据库



	while(1)
	{
		if(recv(sockConn,recvbuf,256,0)>0)
		{
			str1=recvbuf[0];
			if(str1=='0')         //登陆关键字  return 0 1  (成功）
			{
	        	int i=1,j=0;
		        for(i;i<11;i++)
				{
		        	str2[i-1]=recvbuf[i];
				}
	            str2[10]='\0';
				for(i=11;recvbuf[i]!='\0';i++)
				{
					str3[i-11]=recvbuf[i];
					j++;

				}
				str3[j]='\0';

				cout<<str1<<endl<<str2<<endl<<str3<<endl;
				sendbuf[0]=IsExit(conn,str2,str3);
				sendbuf[1]='\0';

				send(sockConn,sendbuf,strlen(sendbuf)+1,0);

			}

	       if(str1=='1')    //查询关键字  return money   (成功）
		   {
	        	int i=1;
	        	for(i;i<11;i++)
				{
		     	    str2[i-1]=recvbuf[i];
				}
		         str2[10]='\0';

		        cout<<str1<<endl<<str2<<endl;

				int money=Getmoney(conn,str2);
				cout<<money<<endl;

				if((money>=0)&&(money<=9))
				{
					sendbuf[0]=money+'0';
					sendbuf[1]='\0';
				}
				else if((money>=10)&&(money<=99))
				{
					sendbuf[1]=(money%10)+'0';
					sendbuf[0]=((money/10)%10)+'0';
					sendbuf[2]='\0';
				}
				else if((money>=100)&&(money<=999))
				{
					sendbuf[2]=(money%10)+'0';
					sendbuf[1]=((money/10)%10)+'0';
					sendbuf[0]=((money/100)%10)+'0';
					sendbuf[3]='\0';
				}
				else if((money>=1000)&&(money<=9999))
				{
					sendbuf[3]=(money%10)+'0';
					sendbuf[2]=((money/10)%10)+'0';
					sendbuf[1]=((money/100)%10)+'0';
					sendbuf[0]=((money/1000)%10)+'0';
					sendbuf[4]='\0';
				}
				cout<<sendbuf<<endl;

				
				send(sockConn,sendbuf,strlen(sendbuf)+1,0);

		   }

		   if(str1=='2')    //取款关键字  return void  (成功)
		   {
				   int i=1,j=0;
				   for(i;i<11;i++)
				   {
				    	str2[i-1]=recvbuf[i];
				   }
				   str2[10]='\0';
			    	for(i=11;recvbuf[i]!='\0';i++)
				   {
				   		   	str3[i-11]=recvbuf[i];
				   		   	j++;
					}
		         str3[j]='\0';

				 int nowmoney=Getmoney(conn,str2);
				 int clientmoney=0;
				 for(i=0;i<strlen(str3);i++)
				 {
	             	j=str3[i]-'0';
	             	clientmoney=clientmoney*10+j;

				 }
				 int finalmoney=nowmoney-clientmoney;


		         cout<<str1<<endl<<str2<<endl<<str3<<endl;
			 /*  sendbuf[0]=UpdateRe(conn,str2,finalmoney);
				 sendbuf[1]='\0';
			*/

				 if(finalmoney<0)
				 {
					 sendbuf[0]='0';
				 }
				 else
					 sendbuf[0]=UpdateRe(conn,str2,finalmoney);

				 sendbuf[1]='\0';


				 send(sockConn,sendbuf,strlen(sendbuf)+1,0);



		   }

		   if(str1=='3')    //存款关键字   return void    (成功)
			 {
				   int i=1,j=0;
				   for(i;i<11;i++)
				   {
				    	str2[i-1]=recvbuf[i];
				   }
				   str2[10]='\0';
			    	for(i=11;recvbuf[i]!='\0';i++)
				   {
				   		   	str3[i-11]=recvbuf[i];
				   		   	j++;
					}
		         str3[j]='\0';

				 int nowmoney=Getmoney(conn,str2);
				 int clientmoney=0;
				 for(i=0;i<strlen(str3);i++)
				 {
	             	j=str3[i]-'0';
	             	clientmoney=clientmoney*10+j;

				 }
				 int finalmoney=nowmoney+clientmoney;

		         cout<<str1<<endl<<str2<<endl<<str3<<endl;
				 sendbuf[0]=UpdateIn(conn,str2,finalmoney);
				 sendbuf[1]='\0';

				 send(sockConn,sendbuf,strlen(sendbuf)+1,0);

		   }

	      if(str1=='4')    //转账关键字   return void
		  {
		       int i=1,j=0;
		       for(i;i<11;i++)
			   {
		        	str2[i-1]=recvbuf[i];
			   }
		       str2[10]='\0';
		       for(i=11;i<21;i++)
			   {
		     	str3[i-11]=recvbuf[i];
			   }
		      str3[10]='\0';
		      for(i=21;recvbuf[i]!='\0';i++)
			  {
		      	str4[i-21]=recvbuf[i];
		     	j++;
			  }
	      	str4[j]='\0';

			int nowmoneyfrom=Getmoney(conn,str2);
			int nowmoneyto=Getmoney(conn,str3);
			int clientmoney=0;
			for(i=0;i<strlen(str4);i++)
			{
	             j=str4[i]-'0';
	             clientmoney=clientmoney*10+j;

			}
			int sqlmoneyfrom=nowmoneyfrom-clientmoney;
			int sqlmoneyto=nowmoneyto+clientmoney;

			sendbuf[0]=UpdateRe(conn,str2,sqlmoneyfrom);
			sendbuf[1]=UpdateIn(conn,str3,sqlmoneyto);
			sendbuf[2]='\0';
			send(sockConn,sendbuf,strlen(sendbuf)+1,0);

		  }

		  if(str1=='e')     //退出
			  break;
		}
	}

	closesocket(sockSer);
	WSACleanup();
	return 0;
}

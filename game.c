#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "game.h"
#include "Player.h"
using namespace std;
#define MAXLINE 4096

//拆分字符
//void splitBuffer(char* buffer, vector<string>& v);
//void split(const string& s, char c, vector<string>& v);
//粘包处理
int processStickPacket(string& sOut, string& sIn);
int main(int argc, char *argv[])
{
	int  client_sockfd;
	struct sockaddr_in  server_addr,client_addr;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	if(inet_pton(AF_INET,argv[1],&server_addr.sin_addr) <= 0)
	{
		printf("inet_pton error for %s\n",argv[1]);
	}
	memset(&client_addr,0,sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(argv[4]));
	if(inet_pton(AF_INET,argv[3],&client_addr.sin_addr) <= 0)
	{
		printf("inet_pton error for %s\n",argv[3]);
	}
	if((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("create socket error for %s(errno:%d)\n",strerror(errno),errno);
	}
	if(bind(client_sockfd,(struct sockaddr*)&client_addr,sizeof(client_addr)) == -1){
		printf("bind socket error for %s(errno:%d)\n",strerror(errno),errno);
	}
	while(connect(client_sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
	{
                usleep(50*1000);
		printf("connect error for %s(errno:%d)\n",strerror(errno),errno);
	}
	string str = "reg:   ";
	str.insert(str.length()-2,argv[5]);	
	str.insert(str.length()-1,"Bond");
	str.insert(str.length(),"eol");
	
	if(send(client_sockfd,str.c_str(),str.length(),0) < 0){
	printf("send msg error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
	}
        
        g_Pid = atoi(argv[5]);
        //进入游戏
        string sIn;
        vector<string> v;
        while(1)
        {
         char buffer[1024] = {'\0'};
         int length = recv(client_sockfd, buffer, sizeof(buffer), 0);
         if(length > 0) 
         {
	     printf("the receive buffer is \n%s\n", buffer);
         string tempstr(buffer);
         sIn += tempstr;
	     string sOut;
         v.clear();
	    while (processStickPacket(sOut, sIn))
             {
                 v.push_back(sOut);
             }
         for (int i = 0; i < v.size(); ++i)
             {
 	          Set_Msg(v[i].c_str(), client_sockfd);
             /* if (v[i].substr(0, 9) == "inquire/ \n")
                 {
                     string str = "call \n";
                     send(client_sockfd, str.c_str(), str.length(), 0);
                 }
                 printf("*********\n%s*************\n", v[i].c_str());*/
             }
        }
        }
	close(client_sockfd);
	exit(0);
}


/*void split(const string& s, char c, vector<string>& v)
{
 string::size_type i = 0;
 string::size_type j = s.find(c);
  if (j == string::npos)
  {
    v.push_back(s);
    return;
   }

 while (j != string::npos)
{
 v.push_back(s.substr(i, j - i));
 i = ++j;
 j = s.find(c, j);

 if (j == string::npos)
   {
    string tempStr = s.substr(i, s.length());
    if (tempStr.length() == 0)
       return;
    else
       v.push_back(tempStr);
   }
}
}

void splitBuffer(char* buffer, vector<string>& v)
{
string tempString(buffer);
vector<string> tempVector;
split(tempString, '\n', tempVector);
for (int i = 0; i < tempVector.size(); ++i)
 {
  split(tempVector[i], ' ', v);
 }

}*/

int processStickPacket(string& sOut, string& sIn)
{
	string::size_type i;
	string tempstr7 = sIn.substr(0, 7);
	string tempstr8 = sIn.substr(0, 8);
	string tempstr10 = sIn.substr(0, 10);
	string tempstr11 = sIn.substr(0, 11);

	if (tempstr7 == "seat/ \n")  //座次消息
	{
		i = sIn.find("/seat \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 7);
		sIn.erase(0, i + 7);
        return 1;
	}
	if (tempstr7 == "hold/ \n")  //手牌信息
	{
		i = sIn.find("/hold \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 7);
		sIn.erase(0, i + 7);
        return 1;
	}
	if (tempstr7 == "flop/ \n")  //公牌消息
	{
		i = sIn.find("/flop \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 7);
		sIn.erase(0, i + 7);
        return 1;
	}
	if (tempstr7 == "turn/ \n")  //转牌消息
	{
		i = sIn.find("/turn \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 7);
		sIn.erase(0, i + 7);
        return 1;
	}
	if (tempstr8 == "river/ \n")  //河牌消息
	{
		i = sIn.find("/river \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 8);
		sIn.erase(0, i + 8);
        return 1;
	}
	if (tempstr8 == "blind/ \n")  //盲注信息
	{
		i = sIn.find("/blind \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 8);
		sIn.erase(0, i + 8);
        return 1;
	}
	if (tempstr10 == "inquire/ \n")  //询问消息
	{
		i = sIn.find("/inquire \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 10);
		sIn.erase(0, i + 10);
        return 1;
	}
	if (tempstr10 == "pot-win/ \n")  //彩池分配消息
	{
		i = sIn.find("/pot-win \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 10);
		sIn.erase(0, i + 10);
        return 1;
	}
	if (tempstr11 == "showdown/ \n")  //摊牌消息
	{
		i = sIn.find("/showdown \n");
		if (i == string::npos)
		{
			return 0;
		}
		sOut = sIn.substr(0, i + 11);
		sIn.erase(0, i + 11);
        return 1;
	}
	if (tempstr11 == "game-over \n")  //游戏结束消息
	{
		sOut = tempstr11;
		sIn.erase(0, 11);
        return 1;
	}
         return 0;
}


#ifndef SCOMLVARSOCKET
#define SCOMLVARSOCKET
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace scoml{
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);

long long getMaxNum(int num);

/****************************************************************
 * 描述: 可变长度 socket 接收函数
 * 输入参数:
 *      handleSocket: socket descript file
 *      msg: 要发送的字符
 *      oriLen; 表达数据字节长度的文本字符数
 * 输出参数:
 *      int: 表示成功与否
****************************************************************/
int varSend(int handleSocket, std::string &msg, int oriLen=12);

/****************************************************************
 * 描述: 可变长度 socket 发送函数
 * 输入参数:
 *      handleSocket: socket descript file
 *      oriLen; 表达数据字节长度的文本字符数
 *      msg: 接收数据变量
 * 输出参数:
 *      int: 表示成功与否
****************************************************************/
int varRecv(int handleSocket, std::string &msg, int oriLen=12);

/****************************************************************
 * 描述: socket 服务端
****************************************************************/
class ServerSocket {
public:
    ServerSocket(std::string iIp, std::string iPort, int iOriLen=12, int iBacklog=10): ip(iIp), port(iPort), oriLen(iOriLen), backlog(iBacklog){}
        

    /****************************************************************
     * 描述: socket 服务端实际初始化函数
    ****************************************************************/
    int init(){
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if ((getaddrinfo(ip.c_str(), port.c_str(), &hints, &p)) != 0){
            std::cout << "getaddrinfo failed." << std::endl;
            return -1;
        }
        if (p == NULL){
            std::cout << "cann't getaddrinfo." << std::endl;
            return -1;
        }
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            std::cout << "socket failed." << std::endl;
            return -1;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            std::cout << "bind failed." << std::endl;
            return -1;
        }
        freeaddrinfo(p);
        if (listen(sockfd, backlog) == -1){
            close(sockfd);
            std::cout << "listen failed." << std::endl;
            return -1;
        }
        std::cout << "listen at " << ":" << port << std::endl;
        return 0;
    }

    /****************************************************************
     * 描述: socket 服务端接收客户端请求函数
    ****************************************************************/
    int cusAccept(){
        sin_size = sizeof(their_addr);
        handleSockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (handleSockfd == -1) {
            std::cout << "one accept failed." << std::endl;
            return -1;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), addrPres, sizeof(addrPres));
        std::cout << "server: got connection from " << addrPres << std::endl;
        return 0;
    }

    std::string response(std::string &msg);

    /****************************************************************
     * 描述: socket 服务端启动函数
    ****************************************************************/
    int start(){
        if(init() != 0){
            std::cout << "init failed." << std::endl;
            return -1;
        }
        while (true){
            if(cusAccept()!=0)
                continue;
            std::string msg;
            if(varRecv(handleSockfd, msg, oriLen)!=0){
                std::cout << "recv error." << std::endl;
                continue;
            }
            std::cout << msg << std::endl;
            std::string answer;
            answer = response(msg);
            if(varSend(handleSockfd, answer, oriLen)!=0){
                std::cout << "send error." << std::endl;
                continue;
            }
            handleClose();
        }
        sockClose();            
        return 0;
    }

    /****************************************************************
     * 描述: 关闭 socket 的实际服务端口
    ****************************************************************/
    int handleClose(){
        close(this->handleSockfd);
        return 0;
    }

    /****************************************************************
     * 描述: 关闭 socket 服务端
    ****************************************************************/
    int sockClose(){
        close(this->sockfd);
        return 0;
    }

    struct addrinfo hints, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    char addrPres[INET6_ADDRSTRLEN];
    std::string ip;
    std::string port;
    int oriLen;
    int backlog;
    int sockfd, handleSockfd;
};

std::string var_request(std::string ip, std::string port, std::string msg, int oriLen=12);

/****************************************************************
 * 描述: socket 客户端类
****************************************************************/
class ClientSocket {
public:
    /****************************************************************
     * 描述: socket 客户端初始化函数
    ****************************************************************/
    ClientSocket(std::string iIp, std::string iPort, int numLen=12):
    ip(iIp), port(iPort), oriLen(numLen){}

    /****************************************************************
     * 描述: socket 客户端实际初始化函数
    ****************************************************************/
    int init(){
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if ((getaddrinfo(ip.c_str(), port.c_str(), &hints, &p)) != 0){
            std::cout << "getaddrinfo failed." << std::endl;
            return -1;
        }
        if (p == NULL){
            std::cout << "cann't getaddrinfo." << std::endl;
            return -1;
        }
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            std::cout << "create client socket failed." << std::endl;
            return -1;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            std::cout << "client connect error." << std::endl;
            return -1;
        }
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), addrPres, sizeof(addrPres));
        std::cout << "client: connecting to " << addrPres << std::endl;
        freeaddrinfo(p);
        return 0;
    }

    /****************************************************************
     * 描述: 关闭 socket 客户端
    ****************************************************************/
    int sockClose(){
        close(this->sockfd);
        return 0;
    }

    struct addrinfo hints, *p;
    char addrPres[INET6_ADDRSTRLEN];
    std::string ip;
    std::string port;
    int oriLen;
    int sockfd;
};
} // namespace end
#endif

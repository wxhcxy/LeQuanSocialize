
#include "network.h"

#define SERV_PORT 6789
#define INFTIM -1 //poll永远等待
#define MAXLINE 9999999
#define INVALID_SOCKET_FD -1

Network::Network()
{

}

int Network::createSocket()
{
    m_sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(m_sockfd==INVALID_SOCKET_FD){
       printf("Create socket failed. Errorn info: %d %s\n",errno,strerror(errno));
    }
//    std::cout<<"Client:: socketFd:"<<m_sockfd<<std::endl;
    return m_sockfd;
}

int Network::connectSocket(const char* ipaddr)
{
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET,ipaddr,&servaddr.sin_addr);

    if(connect(m_sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        printf("Create socket failed. Errorn info: %d %s\n",errno,strerror(errno));
        return -1;
    }
    return 0;
}

bool Network::receiveMessage(char* buffer)
{
    //每次发送数据之前，先发送一个固定长度的自定义包头，包头中定义了这一次数据的长度。
    //服务端先按照包头长度接受包头数据，再按照包头数据中指定的数据长度接受这一次通信的数据。
    //我们使用一个int类型作为“包头”，代表发送数据的长度。
    //而int类型固定4字节，因此服务端每次先接受4字节的数据x，再接受x字节的字符串数据。

    int one_size = 0;
    int msg_size = 0;
    std::cout << "客户端开始接收消息" << std::endl;

    // 接收消息头部长度（4 字节）
    one_size = recv(m_sockfd, &msg_size, sizeof(msg_size), 0);
    if (one_size < 0) {
        std::cerr << "接收消息头部长度失败，错误信息：" << strerror(errno) << std::endl;
        return false;
    } else if (one_size == 0) {
        std::cout << "客户端断开连接" << std::endl;
        return false;
    }

    // 将接收到的消息头部长度从网络字节序转换为本地字节序
    msg_size = ntohl(msg_size);
    std::cout << "客户端接收到的消息头部长度one_size：" << one_size << std::endl;
    std::cout << "客户端接收到的消息头部值长度msg_size：" << msg_size << std::endl;

    int pos = 0;
    while (msg_size > 0) {
        one_size = recv(m_sockfd, buffer + pos, std::min(1024, msg_size), 0);
        if (one_size == 0) {
            std::cout << "客户端断开连接" << std::endl;
            return false;
        }
        if (one_size < 0) {
            std::cerr << "服务器读取错误，错误信息：" << strerror(errno) << std::endl;
            return false;
        }
        pos += one_size;
        msg_size -= one_size;
    }

    std::cout << "消息接收完毕!" << std::endl;
    //std::cout << "收到服务器的消息：" << buffer << std::endl;
    return true;
}

bool Network::sendMessage(const char *buf, size_t size)
{
    if(m_sockfd<0){
        std::cerr<<"Client Socket error"<<std::endl;
        return false;
    }
    int send_size = 0 , msg_size = strlen(buf);

    uint32_t data_length = htonl(msg_size); //主机字节序转化为网络字节序发送给服务器
    std::cout << "msg_size: " << msg_size << std::endl;
    send_size = send(m_sockfd, &data_length, sizeof(data_length), 0);
    std::cout << "send_size1: " << send_size << std::endl;

    // std::cout << "msg_size: " << msg_size << std::endl;
    // send_size = send(m_sockfd, &msg_size, sizeof(4), 0);
    // std::cout << "send_size1: " << send_size << std::endl;

    int pos = 0;
    std::string tmp(buf);
    while (msg_size > 0) {
        send_size = send(m_sockfd, buf + pos, 1024, 0);
        std::cout << "send_size2: " << send_size << std::endl;
        if (send_size < 0) {
            printf("Client write error. Errorn info: %d %s\n",errno,strerror(errno));
            return false;
        }
       pos += send_size;
       msg_size -= send_size;;
       std::cout << "msg_size: " << msg_size << std::endl;
    }
    return true;
}

bool Network::sendFile(char *buf, size_t size, std::string filePath)
{

    if(m_sockfd<0){
        std::cerr<<"Client Socket error"<<std::endl;
        return false;
    }
    FILE *fq;
    if( ( fq = fopen(filePath.c_str(),"rb") ) == NULL ){
        printf("File open.\n");
        close(m_sockfd);
        exit(1);
    }
    int len;
    while(!feof(fq)){
        len = fread(buf, 1, sizeof(buf), fq);
        if(len != ::send(m_sockfd, buf, len,0)){
            printf("write.\n");
            break;
        }
    }
    fclose(fq);
    return true;
}

Network::~Network()
{
    close(m_sockfd);
}

//连接半关闭函数，当前留在套接字发送缓冲区的数据将被发送到，后跟TCP的正常连接终止序列。
//进程不能对当前套接字调用任何写函数。
int Network::closeSocket()
{
    if(shutdown(m_sockfd,SHUT_WR)<0){
        printf("CloseSocket failed. Errorn info: %d %s\n",errno,strerror(errno));
        return -1;
    }
    return 0;
}

std::string Network::receiveFile()
{
    char buf[MAXLINE];
    memset(buf,0,sizeof(buf));
    int n=recv(m_sockfd,buf,sizeof(buf),0);
    std::cout<<"receive size:"<<n;
    std::cout<<buf;
    if( n == -1){
        if(errno == ECONNRESET || errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN){
            printf("Client read error. Errorn info: %d %s\n",errno,strerror(errno));
        }
        return nullptr;
    }else if(n==0){
        printf("The opposite end has closed the socket.\n");
        return nullptr;
    }
    std::string s(buf);
    if(s.empty()){
        std::cerr<<"Network: Client receieve null"<<std::endl;
        return nullptr;
    };
    return s;
}


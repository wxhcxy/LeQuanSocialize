

#include "SocketUtil.h"
#include "Socket.h"
#include <iostream>

//将套接字绑定到指定的 IP 地址和端口
bool SocketUtil::Bind(int sockfd, std::string ip, uint16_t port)
{
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (::bind(sockfd, (struct sockaddr *) &addr, sizeof addr) == SOCKET_ERROR) {
        return false;
    }

    return true;
}

//将套接字设置为非阻塞模式
void SocketUtil::SetNonBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

//将套接字设置为阻塞模式，并可设置写超时时间
void SocketUtil::SetBlock(int fd, int write_timeout)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));
    if (write_timeout > 0) {
#ifdef SO_SNDTIMEO
        struct timeval tv = {write_timeout / 1000, (write_timeout % 1000) * 1000};
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof tv);
#endif
    }
}

//设置套接字地址复用选项
void SocketUtil::SetReuseAddr(int sockfd)
{
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof on);
}

//设置套接字端口复用选项
void SocketUtil::SetReusePort(int sockfd)
{
#ifdef SO_REUSEPORT
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char *) &on, sizeof(on));
#endif
}

//设置 TCP_NODELAY 选项，禁用 Nagle 算法
void SocketUtil::SetNoDelay(int sockfd)
{
#ifdef TCP_NODELAY
    int on = 1;
    int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(on));
#endif
}

//设置 SO_KEEPALIVE 选项，启用 TCP 保活机制
void SocketUtil::SetKeepAlive(int sockfd)
{
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) &on, sizeof(on));
}

//设置 SO_NOSIGPIPE 选项，防止发送 SIGPIPE 信号
void SocketUtil::SetNoSigpipe(int sockfd)
{
#ifdef SO_NOSIGPIPE
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (char *) &on, sizeof(on));
#endif
}

//设置发送缓冲区大小
void SocketUtil::SetSendBufSize(int sockfd, int size)
{
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &size, sizeof(size));
}

//设置接收缓冲区大小
void SocketUtil::SetRecvBufSize(int sockfd, int size)
{
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &size, sizeof(size));
}

//获取对端 IP 地址
std::string SocketUtil::GetPeerIp(int sockfd)
{
    struct sockaddr_in addr = {0};
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if (getpeername(sockfd, (struct sockaddr *) &addr, &addrlen) == 0) {
        return inet_ntoa(addr.sin_addr);
        //使用 inet_ntoa 将 addr.sin_addr 转换为字符串形式的 IP 地址
    }
    return "0.0.0.0";
}

//获取本地套接字的 IP 地址
std::string SocketUtil::GetSocketIp(int sockfd)
{
    struct sockaddr_in addr = {0};
    char str[INET_ADDRSTRLEN] = "127.0.0.1";
    if (GetSocketAddr(sockfd, &addr) == 0) {
        inet_ntop(AF_INET, &addr.sin_addr, str, sizeof(str));
    }
    return str;
}

//获取本地套接字的地址信息
int SocketUtil::GetSocketAddr(int sockfd, struct sockaddr_in *addr)
{
    socklen_t addrlen = sizeof(struct sockaddr_in);
    return getsockname(sockfd, (struct sockaddr *) addr, &addrlen);
    //获取本地地址信息
}

//获取对端端口号
uint16_t SocketUtil::GetPeerPort(int sockfd)
{
    struct sockaddr_in addr = {0};
    socklen_t addrlen = sizeof(struct sockaddr_in);
    //getpeername 获取对端地址信息
    if (getpeername(sockfd, (struct sockaddr *) &addr, &addrlen) == 0) {
        return ntohs(addr.sin_port);
    }
    return 0;
}

//获取对端地址信息
int SocketUtil::GetPeerAddr(int sockfd, struct sockaddr_in *addr)
{
    socklen_t addrlen = sizeof(struct sockaddr_in);
    return getpeername(sockfd, (struct sockaddr *) addr, &addrlen);
}

//关闭套接字
void SocketUtil::Close(int sockfd)
{
    ::close(sockfd);
}

//尝试连接到远程主机，并可设置连接超时时间
bool SocketUtil::Connect(int sockfd, std::string ip, uint16_t port, int timeout)
{
    bool is_connected = true;

    if (timeout > 0) {
        SocketUtil::SetNonBlock(sockfd);
    }

    struct sockaddr_in addr = {0};
    socklen_t addrlen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (::connect(sockfd, (struct sockaddr *) &addr, addrlen) == SOCKET_ERROR) {
        if (timeout > 0) {
            is_connected = false;
            fd_set fd_write;
            FD_ZERO(&fd_write);
            FD_SET(sockfd, &fd_write);
            struct timeval tv = {timeout / 1000, timeout % 1000 * 1000};
            select((int) sockfd + 1, NULL, &fd_write, NULL, &tv);
            //设置写集合：使用 select 检查套接字是否可写，以判断连接是否成功
            if (FD_ISSET(sockfd, &fd_write)) {
                is_connected = true;
            }
            SocketUtil::SetBlock(sockfd);
        } else {
            is_connected = false;
        }
    }

    return is_connected;
}

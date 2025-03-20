

#include "TcpSocket.h"
#include "Logger.h"
#include "Socket.h"
#include "SocketUtil.h"

TcpSocket::TcpSocket(int sockfd)
    : sockfd_(sockfd)
{}

TcpSocket::~TcpSocket() {}

//创建一个新的 TCP 套接字
int TcpSocket::Create()
{
    //调用底层socket 函数，参数 AF_INET 表示使用 IPv4，SOCK_STREAM 表示使用 TCP 协议，0 表示默认协议
    sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    return sockfd_;
}

//将套接字绑定到指定的 IP 地址和端口
bool TcpSocket::Bind(std::string ip, uint16_t port)
{
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;                    //使用 IPv4
    addr.sin_addr.s_addr = inet_addr(ip.c_str()); //将字符串形式的 IP 地址转换为网络字节序
    addr.sin_port = htons(port);                  //将端口号转换为网络字节序

    if (::bind(sockfd_, (struct sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR) { //如果绑定失败
        LOG_DEBUG(" <socket=%d> bind <%s:%u> failed.\n", sockfd_, ip.c_str(), port);
        return false;
    }

    return true;
}

//开始监听传入的连接请求，backlog参数指定挂起连接队列的最大长度
bool TcpSocket::Listen(int backlog)
{
    if (::listen(sockfd_, backlog) == SOCKET_ERROR) {
        LOG_DEBUG("<socket=%d> listen failed.\n", sockfd_);
        return false;
    }
    std::cout << "TcpSocket::Listen()" << std::endl;

    return true;
}

//接受一个传入的连接请求，返回一个新的套接字描述符用于与客户端通信
int TcpSocket::Accept()
{
    struct sockaddr_in addr = {0};
    socklen_t addrlen = sizeof addr;

    int socket_fd = ::accept(sockfd_, (struct sockaddr *) &addr, &addrlen);
    return socket_fd;
    //返回与客户端通信的新套接字描述符
}

//尝试连接到远程主机，参数包括 IP 地址、端口和可选的超时时间（以毫秒为单位）
bool TcpSocket::Connect(std::string ip, uint16_t port, int timeout)
{
    //timeout 指定连接的超时时间（以毫秒为单位）
    if (!SocketUtil::Connect(sockfd_, ip, port, timeout)) {
        LOG_DEBUG("<socket=%d> connect failed.\n", sockfd_);
        return false;
    }

    return true;
}

//关闭套接字，释放资源
void TcpSocket::Close()
{
    ::close(sockfd_);
    sockfd_ = 0;
}

//关闭套接字的写半部，用于优雅地关闭连接
void TcpSocket::ShutdownWrite()
{
    shutdown(sockfd_, SHUT_WR);
    sockfd_ = 0;
}

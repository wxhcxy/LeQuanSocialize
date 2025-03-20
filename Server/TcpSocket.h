

#ifndef XOP_TCP_SOCKET_H
#define XOP_TCP_SOCKET_H

#include "Socket.h"
#include <cstdint>
#include <string>

class TcpSocket
{
public:
    TcpSocket(int sockfd = -1);
    virtual ~TcpSocket();

    int Create();
    bool Bind(std::string ip, uint16_t port);
    bool Listen(int backlog);
    int Accept();
    bool Connect(std::string ip, uint16_t port, int timeout = 0);
    void Close();
    void ShutdownWrite();
    int GetSocket() const { return sockfd_; }
    //返回当前的套接字描述符

private:
    int sockfd_ = -1;
};

#endif

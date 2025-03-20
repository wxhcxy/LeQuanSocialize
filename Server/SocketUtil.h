

#ifndef XOP_SOCKET_UTIL_H
#define XOP_SOCKET_UTIL_H

#include <arpa/inet.h>
#include <string>

class SocketUtil
{
public:
    static bool Bind(int sockfd, std::string ip, uint16_t port);
    static void SetNonBlock(int fd);
    static void SetBlock(int fd, int write_timeout = 0);
    static void SetReuseAddr(int fd);
    static void SetReusePort(int sockfd);
    static void SetNoDelay(int sockfd);
    static void SetKeepAlive(int sockfd);
    static void SetNoSigpipe(int sockfd);
    static void SetSendBufSize(int sockfd, int size);
    static void SetRecvBufSize(int sockfd, int size);
    static std::string GetPeerIp(int sockfd);
    static std::string GetSocketIp(int sockfd);
    static int GetSocketAddr(int sockfd, struct sockaddr_in *addr);
    static uint16_t GetPeerPort(int sockfd);
    static int GetPeerAddr(int sockfd, struct sockaddr_in *addr);
    static void Close(int sockfd);
    static bool Connect(int sockfd, std::string ip, uint16_t port, int timeout = 0);
};

#endif // _SOCKET_UTIL_H

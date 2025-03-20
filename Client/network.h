
#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>

#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

class Network {
public:
    Network();
    int createSocket();
    int connectSocket(const char *ipaddr);
    int closeSocket();

    std::string receiveFile();

    bool receiveMessage(char* buffer);
    bool sendMessage(const char *buf, size_t size);
    bool sendFile(char *buf,size_t size,std::string filePath);
    ~Network();
private:
    int m_sockfd;

};

#endif // NETWORK_H

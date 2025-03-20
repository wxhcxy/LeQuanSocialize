

#ifndef XOP_TCPSERVER_H
#define XOP_TCPSERVER_H

#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>
#include "Socket.h"
#include "TcpConnection.h"

class Acceptor;
class EventLoop;

class TcpServer
{
public:	
	TcpServer(EventLoop* event_loop);
	virtual ~TcpServer();  

	virtual bool Start(std::string ip, uint16_t port);
	virtual void Stop();

protected:
    virtual TcpConnection::Ptr OnConnect(int sockfd);
    virtual void AddConnection(int sockfd, TcpConnection::Ptr tcp_conn);
    virtual void RemoveConnection(int sockfd);

    EventLoop* event_loop_;
	uint16_t port_;
	std::string ip_;
    std::unique_ptr<Acceptor> acceptor_; //指向 Acceptor 对象，负责接受新连接
    bool is_started_;                    //指示服务器是否已启动
    std::mutex mutex_;
    std::unordered_map<int, TcpConnection::Ptr> connections_;
    //无序映射，存储套接字描述符与对应的 TcpConnection 对象之间的关联
};

#endif 

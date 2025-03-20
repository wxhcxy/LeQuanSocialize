#ifndef XOP_ACCEPTOR_H
#define XOP_ACCEPTOR_H

#include "Channel.h"
#include "TcpSocket.h"
#include <functional>
#include <memory>
#include <mutex>

//表示接受新连接时的回调函数。该回调函数接受一个参数 SOCKET，表示新连接的套接字描述符
typedef std::function<void(int)> NewConnectionCallback;

class EventLoop;

class Acceptor
{
public:	
	Acceptor(EventLoop* eventLoop);
	virtual ~Acceptor();

    //设置新连接回调函数cb，当有新连接时，Acceptor 会调用该回调函数，传递新连接的套接字描述符
    void SetNewConnectionCallback(const NewConnectionCallback& cb)
    {
        new_connection_callback_ = cb;
    }

    int Listen(std::string ip, uint16_t port);
    void Close();

private:
	void OnAccept();

	EventLoop* event_loop_ = nullptr;
	std::mutex mutex_;
    std::unique_ptr<TcpSocket> tcp_socket_; //指向 TcpSocket 对象，用于处理 TCP 套接字操作
    ChannelPtr channel_ptr_; //智能指针，指向 Channel 对象，用于监听套接字的可读事件
    NewConnectionCallback new_connection_callback_; //新连接回调函数，当有新连接时调用
};

#endif 

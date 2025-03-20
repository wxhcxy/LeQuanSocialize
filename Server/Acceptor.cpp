#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketUtil.h"
#include "Logger.h"

Acceptor::Acceptor(EventLoop* eventLoop)
    : event_loop_(eventLoop)
    , tcp_socket_(new TcpSocket)
{	
	
}

Acceptor::~Acceptor()
{

}

//开始监听指定的 IP 地址和端口。返回整数，表示操作是否成功（0 表示成功，负数表示失败）
int Acceptor::Listen(std::string ip, uint16_t port)
{
	std::lock_guard<std::mutex> locker(mutex_);

    if (tcp_socket_->GetSocket() > 0) { //如果 tcp_socket_ 已存在套接字描述符，则关闭它
        tcp_socket_->Close();
    }

    int sockfd = tcp_socket_->Create();      //创建新的套接字描述符 sockfd
    channel_ptr_.reset(new Channel(sockfd)); //创建 Channel 对象 channel_ptr_，关联到 sockfd
    SocketUtil::SetReuseAddr(sockfd);        //设置地址复用
    SocketUtil::SetReusePort(sockfd);        //设置端口复用
    SocketUtil::SetNonBlock(sockfd);         //设置非阻塞模式

    //绑定套接字到指定的 IP 地址和端口
    if (!tcp_socket_->Bind(ip, port)) {
        return -1;
    }

    //开始监听，参数 1024 表示监听队列的长度
    if (!tcp_socket_->Listen(1024)) {
        return -1;
    }

    std::cout << "Acceptor::Listen()" << std::endl;

    //设置 channel_ptr_ 的读取回调函数为 OnAccept() 方法
    channel_ptr_->SetReadCallback([this]() { this->OnAccept(); });
    channel_ptr_->EnableReading();            //启用通道的读取功能
    event_loop_->UpdateChannel(channel_ptr_); //将通道添加到事件循环中，开始监听可读事件
    return 0;
}

//关闭监听器，停止接受新连接
void Acceptor::Close()
{
	std::lock_guard<std::mutex> locker(mutex_);

    //移除通道：如果套接字描述符有效，则从事件循环中移除通道
    if (tcp_socket_->GetSocket() > 0) {
        event_loop_->RemoveChannel(channel_ptr_);
        tcp_socket_->Close();
    }
}

//当有新连接到来时调用，处理新连接
void Acceptor::OnAccept()
{
	std::lock_guard<std::mutex> locker(mutex_);

    std::cout << "Acceptor::OnAccept()" << std::endl;

    int socket = tcp_socket_->Accept();    //接受新连接，返回新的套接字描述符 socket
    std::cout << "socket: " << socket << std::endl;

    if (socket > 0) {                      //大于 0，表示接受连接成功
        if (new_connection_callback_) {
            new_connection_callback_(socket); //调用回调函数
        } else {
            SocketUtil::Close(socket); //如果回调函数为空，则关闭套接字
        }
    }
}


#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Logger.h"
#include <cstdio>

using namespace std;

//接受一个指向 EventLoop 的指针，用于事件循环管理
TcpServer::TcpServer(EventLoop* event_loop)
    : event_loop_(event_loop)
    , port_(0)
    , acceptor_(new Acceptor(event_loop_))
    , is_started_(false) //初始化为 false，表示服务器尚未启动
{
    acceptor_->SetNewConnectionCallback([this](int sockfd) { //设置新连接回调函数
        std::cout << "这是接受到新的连接的回调函数，新的客户端连接过来时触发TcpServer::TcpServer"
                  << std::endl;
        std::cout << "!!!TcpServer::TcpServer(EventLoop* event_loop)线程标识：: "
                  << std::this_thread::get_id() << std::endl;
        TcpConnection::Ptr conn = this->OnConnect(sockfd);
        if (conn) {
            this->AddConnection(sockfd, conn);
            conn->SetDisconnectCallback([this](TcpConnection::Ptr conn) { //设置断开连接回调函数
                auto scheduler = conn->GetTaskScheduler();
                int sockfd = conn->GetSocket();
                if (!scheduler->AddTriggerEvent(
                        [this, sockfd] { this->RemoveConnection(sockfd); })) {
                    scheduler->AddTimer(
                        [this, sockfd]() {
                            this->RemoveConnection(sockfd);
                            return false;
                        },
                        100);
                }
            });
        }
    });
}

TcpServer::~TcpServer()
{
	Stop();
}

//启动 TCP 服务器，监听指定的 IP 地址和端口
bool TcpServer::Start(std::string ip, uint16_t port)
{
    //Stop(); //停止现有服务器

    if (!is_started_) {
        std::cout << "TcpServer::Start" << std::endl;
        if (acceptor_->Listen(ip, port) < 0) {
            return false;
        }

        port_ = port;
        ip_ = ip;
        is_started_ = true;
        return true;
    }

    return false;
}

//停止 TCP 服务器，关闭监听和所有现有连接
void TcpServer::Stop()
{
    if (is_started_) {
        mutex_.lock();
        for (auto iter : connections_) { //断开所有连接
            iter.second->Disconnect();
        }
        mutex_.unlock();

        acceptor_->Close();  //关闭监听器，停止接受新连接
        is_started_ = false; //设置为 false，表示服务器已停止

        //等待所有连接断开
        while (1) {
            Timer::Sleep(10);
            if (connections_.empty()) {
                break;
            }
        }
    }
}

//处理新连接
//当有新连接建立时调用。返回一个指向 TcpConnection 的智能指针，用于管理该连接
TcpConnection::Ptr TcpServer::OnConnect(int sockfd)
{
    std::cout << "有新连接到来，处理新连接TcpServer::OnConnect" << std::endl;
    //使用 std::make_shared 创建一个 TcpConnection 对象，
    //传入任务调度器（通过 event_loop_->GetTaskScheduler().get() 获取）和套接字描述符 sockfd
    return std::make_shared<TcpConnection>(event_loop_->GetTaskScheduler().get(), sockfd);
}

//用于添加一个新的 TCP 连接。参数包括套接字描述符和对应的 TcpConnection 对象
void TcpServer::AddConnection(int sockfd, TcpConnection::Ptr tcpConn)
{
    std::lock_guard<std::mutex> locker(mutex_);
    connections_.emplace(sockfd, tcpConn);
    //将套接字描述符 sockfd 和对应的 TcpConnection 对象 tcpConn 添加到 connections_ 中
    std::cout << "添加一个连接，添加后Tcp连接数量 connections_ size: " << connections_.size()
              << std::endl;
}

//用于移除一个 TCP 连接
void TcpServer::RemoveConnection(int sockfd)
{
    std::lock_guard<std::mutex> locker(mutex_);
    connections_.erase(sockfd);
    std::cout << "移除一个连接，移除后Tcp连接数量 connections_ size: " << connections_.size()
              << std::endl;
}

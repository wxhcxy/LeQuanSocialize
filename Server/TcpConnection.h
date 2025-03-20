#ifndef XOP_TCP_CONNECTION_H
#define XOP_TCP_CONNECTION_H

#include <atomic>
#include <mutex>
#include "TaskScheduler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Channel.h"
#include "SocketUtil.h"

//测试，可能TcpConnection里不应用关联控制层
#include "controllerfactory.h"

//继承自 std::enable_shared_from_this：允许在类内部获取自身的 std::shared_ptr，用于在回调函数中安全地持有自身的引用
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
	using Ptr = std::shared_ptr<TcpConnection>;
    //断开连接时的回调函数类型
    using DisconnectCallback = std::function<void(std::shared_ptr<TcpConnection> conn)>;
    //关闭连接时的回调函数类型
    using CloseCallback = std::function<void(std::shared_ptr<TcpConnection> conn)>;
    //读取数据时的回调函数类型，BufferReader 对象的引用作为参数，返回布尔值表示是否继续读取。
    using ReadCallback
        = std::function<bool(std::shared_ptr<TcpConnection> conn, BufferReader& buffer)>;

    TcpConnection(TaskScheduler* task_scheduler, int sockfd);
    virtual ~TcpConnection();

    //返回指向任务调度器的指针 task_scheduler_，用于在需要时访问任务调度器
    TaskScheduler* GetTaskScheduler() const { return task_scheduler_; }

    //设置读取数据的回调函数 read_cb_，当有数据可读时，会调用该回调函数
    void SetReadCallback(const ReadCallback& cb) { read_cb_ = cb; }

    //设置关闭连接的回调函数 close_cb_，当连接关闭时，会调用该回调函数
    void SetCloseCallback(const CloseCallback& cb) { close_cb_ = cb; }

    void Send(std::shared_ptr<char> data, uint32_t size); //发送数据
    void Send(const char* data, uint32_t size);

    void Disconnect(); //断开连接，优雅地关闭连接

    bool IsClosed() const { return is_closed_; }

    int GetSocket() const { return channel_->GetSocket(); }

    uint16_t GetPort() const { return SocketUtil::GetPeerPort(channel_->GetSocket()); }

    std::string GetIp() const { return SocketUtil::GetPeerIp(channel_->GetSocket()); }

protected:
	friend class TcpServer;

    virtual void HandleRead();  //处理读取事件，当有数据可读时调用
    virtual void HandleWrite(); //处理写入事件，当可以写入数据时调用
    virtual void HandleClose(); //处理连接关闭事件
    virtual void HandleError(); //处理错误事件

    //设置断开连接的回调函数 disconnect_cb_
    void SetDisconnectCallback(const DisconnectCallback& cb) { disconnect_cb_ = cb; }

    TaskScheduler* task_scheduler_;
    std::unique_ptr<BufferReader> read_buffer_;
    std::unique_ptr<BufferWriter> write_buffer_;
    std::atomic_bool is_closed_;

private:
    void Close(); //关闭连接

    std::shared_ptr<Channel> channel_;
    std::mutex mutex_;

    //回调函数，用于处理断开连接、关闭连接和读取数据事件
    DisconnectCallback disconnect_cb_;
	CloseCallback close_cb_;
	ReadCallback read_cb_;

    //测试，可能TcpConnection里不应用关联控制层
    ScanAndCheckJottingController* m_scanAndCheckJottingController;
    InitController* m_initController;
    PublishJottingController* m_publishJottingController; //发送评论相关
    MessageController* m_messageController;
    CacheFlush* m_cacheFlush;
};

#endif 

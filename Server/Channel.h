
#ifndef XOP_CHANNEL_H
#define XOP_CHANNEL_H

#include <functional>
#include <memory>
#include "Socket.h"

enum EventType {
    EVENT_NONE = 0,    //无事件
    EVENT_IN = 1,      //可读事件
    EVENT_PRI = 2,     //优先级事件
    EVENT_OUT = 4,     //可写事件
    EVENT_ERR = 8,     //错误事件
    EVENT_HUP = 16,    //挂断事件，表示连接被对端关闭
    EVENT_RDHUP = 8192 //对端关闭写半部事件
};

class Channel 
{
public:
    //定义回调类型
    //EventCallback 是一个类型别名，代表一个函数对象(std::function)，该函数不接受任何参数(void())并且不返回任何值(void)
    typedef std::function<void()> EventCallback;

    Channel() = delete; //删除默认构造函数

    Channel(int sockfd)
        : sockfd_(sockfd)
    {}

    virtual ~Channel(){};

    //设置可读事件的回调函数 read_callback_
    void SetReadCallback(const EventCallback& cb)
    {
        read_callback_ = cb;
    }

    //设置可写事件的回调函数
    void SetWriteCallback(const EventCallback& cb) { write_callback_ = cb; }

    //设置关闭事件的回调函数
    void SetCloseCallback(const EventCallback& cb) { close_callback_ = cb; }

    //设置错误事件的回调函数
    void SetErrorCallback(const EventCallback& cb) { error_callback_ = cb; }

    int GetSocket() const { return sockfd_; }

    int GetEvents() const { return events_; }        //返回当前注册的事件标志
    void SetEvents(int events) { events_ = events; } //设置事件标志

    //启用可读事件，将 EVENT_IN 标志添加到 events_ 中
    void EnableReading() { events_ |= EVENT_IN; }

    //启用可写事件，将 EVENT_OUT 标志添加到 events_ 中
    void EnableWriting() { events_ |= EVENT_OUT; }

    //禁用可读事件，从 events_ 中清除 EVENT_IN 标志
    void DisableReading() { events_ &= ~EVENT_IN; }

    //禁用可写事件，从 events_ 中清除 EVENT_OUT 标志
    void DisableWriting() { events_ &= ~EVENT_OUT; }

    bool IsNoneEvent() const { return events_ == EVENT_NONE; } //检查是否没有注册任何事件
    bool IsWriting() const { return (events_ & EVENT_OUT) != 0; } //检查是否注册了可写事件
    bool IsReading() const { return (events_ & EVENT_IN) != 0; } //检查是否注册了可读事件

    //处理事件：根据传入的事件标志events，调用相应的回调函数：
    void HandleEvent(int events)
    {
        //可读事件：如果事件包含 EVENT_PRI 或 EVENT_IN，则调用 read_callback_()
        if (events & (EVENT_PRI | EVENT_IN)) { //可读事件
            read_callback_();
            std::cout << "调用读回调read_callback_() Channel::HandleEvent(int events)" << std::endl;
        }

        if (events & EVENT_OUT) { //可写事件
            write_callback_();
            std::cout << "调用写回调write_callback_() Channel::HandleEvent(int events)"
                      << std::endl;
        }

        if (events & EVENT_HUP) { //挂断事件
            close_callback_();
            std::cout << "调用关闭回调close_callback_() Channel::HandleEvent(int events)"
                      << std::endl;
            return;
        }

        if (events & (EVENT_ERR)) { //错误事件
            error_callback_();
            std::cout << "调用错误事件回调error_callback_() Channel::HandleEvent(int events)"
                      << std::endl;
        }
    }

private:
    //初始化回调函数，为四个成员变量分配内存，并使用默认的 Lambda 表达式进行初始化
    EventCallback read_callback_ = [] {};
    EventCallback write_callback_ = []{};
	EventCallback close_callback_ = []{};
	EventCallback error_callback_ = []{};

    int sockfd_ = 0;
    int events_ = 0; //events_：默认初始化为 0，表示无事件
};

//定义 ChannelPtr 类型为指向 Channel 的智能指针，方便管理 Channel 对象
typedef std::shared_ptr<Channel> ChannelPtr;

#endif  


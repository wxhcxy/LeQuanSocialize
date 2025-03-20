

#ifndef XOP_EVENT_LOOP_H
#define XOP_EVENT_LOOP_H

#include <atomic>     //原子操作，支持线程安全的操作
#include <functional> //函数对象，支持回调函数
#include <memory>
#include <mutex>
#include <queue> //队列容器，用于任务调度
#include <thread>
#include <unordered_map>

#include "EpollTaskScheduler.h"

class EventLoop 
{
public:
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    EventLoop(uint32_t num_threads = 1); //std::thread::hardware_concurrency()//硬件支持的并发线程数

    virtual ~EventLoop();

    std::shared_ptr<TaskScheduler> GetTaskScheduler();

    //添加一个触发事件回调函数 callback，当事件被触发时调用该回调函数。TriggerEvent 是函数类型，通常定义为 std::function<void()>
    bool AddTriggerEvent(TriggerEvent callback);

    //更新一个通道（Channel）的状态。ChannelPtr 是指向 Channel 对象的智能指针。通道通常用于表示网络连接、文件描述符等。
    void UpdateChannel(ChannelPtr channel);

    //移除一个通道，停止对其事件的管理。
    void RemoveChannel(ChannelPtr &channel);

    void Loop(); //启动事件循环，开始处理事件和任务
    void Quit(); //停止事件循环，退出循环

private:
	std::mutex mutex_;
    uint32_t num_threads_ = 1; //事件循环中使用的线程数
    uint32_t index_ = 1;       //用于在多线程环境下选择任务调度器的索引
    std::vector<std::shared_ptr<TaskScheduler>> task_schedulers_; //存储任务调度器实例
    std::vector<std::shared_ptr<std::thread>> threads_;           //存储线程实例
};

#endif


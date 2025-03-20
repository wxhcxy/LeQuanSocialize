

#ifndef XOP_TASK_SCHEDULER_H
#define XOP_TASK_SCHEDULER_H

#include "Channel.h"
#include "Pipe.h"
#include "RingBuffer.h"
#include "Timer.h"

//定义 TriggerEvent 类型为 std::function<void(void)>，表示无参数、无返回值的回调函数，用于表示触发事件
typedef std::function<void(void)> TriggerEvent;

class TaskScheduler 
{
public:
    TaskScheduler(int id = 1); //接受一个可选参数 id，用于标识任务调度器实例。默认值为1。
    virtual ~TaskScheduler();

    void Start(); //启动任务调度器，开始处理任务
    void Stop();  //停止任务调度器，停止处理任务

    //添加一个定时器事件 timerEvent，在 msec 毫秒后触发
    TimerId AddTimer(TimerEvent timerEvent, uint32_t msec);
    void RemoveTimer(TimerId timerId);
    bool AddTriggerEvent(TriggerEvent callback);

    //更新通道状态。默认实现为空，派生类可以重写该方法以实现具体功能
    virtual void UpdateChannel(ChannelPtr channel){};
    virtual void RemoveChannel(ChannelPtr& channel){};
    virtual bool HandleEvent(int timeout) { return false; };

    int GetId() const { return id_; } //返回任务调度器的唯一标识符 id_

protected:
    void Wake();               //唤醒任务调度器，通知其有新的事件需要处理
    void HandleTriggerEvent(); //处理触发事件

    int id_ = 0;                   //任务调度器的唯一标识符
    std::atomic_bool is_shutdown_; //原子布尔变量，用于指示任务调度器是否已停止
    std::unique_ptr<Pipe> wakeup_pipe_; //指向 Pipe 对象，用于在事件循环中唤醒任务调度器
    std::shared_ptr<Channel> wakeup_channel_; //用于监听 wakeup_pipe_ 的读取事件
    std::unique_ptr<RingBuffer<TriggerEvent>> trigger_events_;
    //指向 RingBuffer<TriggerEvent> 对象，用于存储触发事件回调函数

    std::mutex mutex_;
    TimerQueue timer_queue_; //定时器队列，用于管理定时器事件

    //kTriggetEvent 和 kTimerEvent：用于区分不同类型的事件
    static const char kTriggetEvent = 1;
    static const char kTimerEvent = 2;
    static const int kMaxTriggetEvents = 50000;
    //kMaxTriggetEvents：环形缓冲区中触发事件的最大数量
};

#endif  


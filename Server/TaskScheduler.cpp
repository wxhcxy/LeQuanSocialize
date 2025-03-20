#include "TaskScheduler.h"
#include <signal.h>

TaskScheduler::TaskScheduler(int id)
    : id_(id)
    , is_shutdown_(false)      //初始化为 false，表示任务调度器正在运行
    , wakeup_pipe_(new Pipe()) //创建 Pipe 对象，用于在事件循环中唤醒任务调度器
    , trigger_events_(new RingBuffer<TriggerEvent>(kMaxTriggetEvents))
//创建 RingBuffer<TriggerEvent> 对象，用于存储触发事件回调函数
{
    std::cout << "!!!TaskScheduler::TaskScheduler(int id)线程标识: " << std::this_thread::get_id()
              << std::endl;
    static std::once_flag flag;
    std::call_once(flag, [] {
	});

    if (wakeup_pipe_->Create()) {
        wakeup_channel_.reset(new Channel(wakeup_pipe_->Read()));
        wakeup_channel_->EnableReading();
        wakeup_channel_->SetReadCallback([this]() { this->Wake(); });
    }
}

TaskScheduler::~TaskScheduler()
{
	
}

//创建唤醒管道
void TaskScheduler::Start()
{
    std::cout << "!!!TaskScheduler::Start()线程标识: " << std::this_thread::get_id() << std::endl;
    std::cout << "TaskScheduler::Start() in " << std::endl;
    //忽略信号：忽略 SIGPIPE、SIGQUIT、SIGUSR1、SIGTERM 和 SIGKILL 信号，防止程序因信号而意外终止
    signal(SIGPIPE, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGKILL, SIG_IGN);
    is_shutdown_ = false; //设置为 false，表示任务调度器正在运行
    while (!is_shutdown_) {
        this->HandleTriggerEvent();
        this->timer_queue_.HandleTimerEvent();
        int64_t timeout = this->timer_queue_.GetTimeRemaining();
        this->HandleEvent((int) timeout);
    }
}

//停止任务调度器
void TaskScheduler::Stop()
{
    is_shutdown_ = true; //设置为 true，指示任务调度器停止运行
    char event = kTriggetEvent;
    wakeup_pipe_->Write(&event, 1);
    //唤醒任务调度器：
    //向 wakeup_pipe_ 写入一个字节的数据，触发 Wake() 方法，
    //唤醒事件循环，以便其检测到 is_shutdown_ 标志并退出循环
}

//添加定时器
TimerId TaskScheduler::AddTimer(TimerEvent timerEvent, uint32_t msec)
{
    TimerId id = timer_queue_.AddTimer(timerEvent, msec);
    return id;
}

//移除定时器
void TaskScheduler::RemoveTimer(TimerId timerId)
{
    timer_queue_.RemoveTimer(timerId);
}

//添加触发事件
bool TaskScheduler::AddTriggerEvent(TriggerEvent callback)
{
    //检查缓冲区容量：如果 trigger_events_ 中未处理的触发事件数量小于 kMaxTriggetEvents，则继续添加；否则，拒绝添加
    if (trigger_events_->Size() < kMaxTriggetEvents) {
        std::lock_guard<std::mutex> lock(mutex_);
        char event = kTriggetEvent;
        trigger_events_->Push(std::move(callback));
        //添加事件：将 callback 回调函数添加到 trigger_events_ 中

        wakeup_pipe_->Write(&event, 1);
        return true;
    }

    return false;
}

//唤醒任务调度器
void TaskScheduler::Wake()
{
	char event[10] = { 0 };
	while (wakeup_pipe_->Read(event, 10) > 0);
    //读取 wakeup_pipe_ 中的所有数据，清空管道内容。这是为了确保 wakeup_channel_ 的读取回调函数被触发，从而唤醒事件循环
}

//处理触发事件
void TaskScheduler::HandleTriggerEvent()
{
    std::cout << "TaskScheduler::HandleTriggerEvent()" << std::endl;
    do 
	{
		TriggerEvent callback;
		if (trigger_events_->Pop(callback)) {
			callback();
		}
	} while (trigger_events_->Size() > 0);

    /*
     * 循环处理事件：重复执行以下步骤，直到所有触发事件都被处理：
            弹出事件：从 trigger_events_ 中弹出一个触发事件回调函数 callback。
            调用回调函数：调用 callback()，执行事件处理逻辑。
    注意：这里假设 Pop 方法会阻塞，直到有事件可弹出，或者返回 false 表示没有事件可处理
    */
}

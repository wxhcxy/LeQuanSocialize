

#include "EventLoop.h"

EventLoop::EventLoop(uint32_t num_threads)
    : index_(1) //将 index_ 初始化为1，用于选择任务调度器
{
    //设置线程数： 默认情况下，num_threads_ 设置为1
    num_threads_ = 1;
    if (num_threads > 0) { //如果传入的 num_threads 大于0，则将其赋值给 num_threads_
        num_threads_ = num_threads;
    }

    this->Loop(); // 启动事件循环：调用 Loop() 方法启动事件循环。
}

EventLoop::~EventLoop()
{
    this->Quit(); //在对象销毁时，调用 Quit() 方法停止事件循环，确保资源被正确释放
}

//获取任务调度器
std::shared_ptr<TaskScheduler> EventLoop::GetTaskScheduler()
{
	std::lock_guard<std::mutex> locker(mutex_);
	if (task_schedulers_.size() == 1) {
		return task_schedulers_.at(0);
    }

    //如果任务调度器数量大于1，则使用 index_ 轮询选择任务调度器，并递增 index_，如果 index_ 超过调度器数量，则重置为1
    else {
        std::cout << "返回调度器：" << index_ << std::endl;
        auto task_scheduler = task_schedulers_.at(index_);
        index_++;
        if (index_ >= task_schedulers_.size()) {
            index_ = 1;
        }
        std::cout << "!!!EventLoop::GetTaskScheduler()线程标识: " << std::this_thread::get_id()
                  << std::endl;
        return task_scheduler; //返回指向 TaskScheduler 的智能指针
    }

    return nullptr;
}

//启动事件循环
void EventLoop::Loop()
{
	std::lock_guard<std::mutex> locker(mutex_);

    //检查是否已启动：如果 task_schedulers_ 不为空，说明事件循环已启动，直接返回。
    if (!task_schedulers_.empty()) {
        return;
    }

    //循环创建 num_threads_ 个 TaskScheduler 实例
    //这里使用的是 EpollTaskScheduler，表示使用 Linux 的 epoll 机制进行事件管理。
    for (uint32_t n = 0; n < num_threads_; n++) {
        //多态，基类指针指向派生类对象
        std::shared_ptr<TaskScheduler> task_scheduler_ptr(new EpollTaskScheduler(n));
        task_schedulers_.push_back(task_scheduler_ptr);

        //创建新线程，并将任务调度器的 Start 方法作为线程的入口点
        std::shared_ptr<std::thread> thread(
            new std::thread(&TaskScheduler::Start, task_scheduler_ptr.get()));
        thread->native_handle(); //调用 native_handle() 获取线程的原生句柄（用于与操作系统交互）
        threads_.push_back(thread);
    }

    for (auto iter : threads_) {
        std::cout << "线程 ID: " << iter->get_id() << std::endl;
    }
}

//停止事件循环
void EventLoop::Quit()
{
	std::lock_guard<std::mutex> locker(mutex_);

    //遍历 task_schedulers_，调用每个调度器的 Stop() 方法，停止任务调度
    for (auto iter : task_schedulers_) {
        iter->Stop();
    }

    //等待线程结束：遍历 threads_，调用每个线程的 join() 方法，等待线程结束
    for (auto iter : threads_) {
        iter->join();
    }

    //清空 task_schedulers_ 和 threads_ 容器，释放资源
    task_schedulers_.clear();
    threads_.clear();
}

//更新通道
void EventLoop::UpdateChannel(ChannelPtr channel)
{
    std::cout << "EventLoop::UpdateChannel(ChannelPtr channel)" << std::endl;
    std::cout << "任务调度器数量 task_schedulers_：" << task_schedulers_.size() << std::endl;
    //任务调度器数量和线程数量是大小一样的

    std::lock_guard<std::mutex> locker(mutex_);
    if (task_schedulers_.size() > 0) {
        task_schedulers_[0]->UpdateChannel(channel);
        //调用第一个任务调度器的 UpdateChannel 方法，更新指定通道的状态
        //注意：这里假设 task_schedulers_ 中至少有一个调度器，并且只更新第一个调度器管理的通道
    }
}

void EventLoop::RemoveChannel(ChannelPtr& channel)
{
	std::lock_guard<std::mutex> locker(mutex_);
	if (task_schedulers_.size() > 0) {
		task_schedulers_[0]->RemoveChannel(channel);
    }
}

//添加触发事件
bool EventLoop::AddTriggerEvent(TriggerEvent callback)
{   
	std::lock_guard<std::mutex> locker(mutex_);
	if (task_schedulers_.size() > 0) {
		return task_schedulers_[0]->AddTriggerEvent(callback);
        //添加触发事件：调用第一个任务调度器的 AddTriggerEvent 方法，添加一个触发事件回调函数 callback
    }
    return false;
}

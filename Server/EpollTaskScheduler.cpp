

#include "EpollTaskScheduler.h"

#include <sys/epoll.h>
#include <errno.h>

//接受一个可选参数id，用于标识任务调度器实例
EpollTaskScheduler::EpollTaskScheduler(int id)
	: TaskScheduler(id)
{
    std::cout << "线程标识1: " << std::this_thread::get_id() << std::endl;
    //创建 epoll 实例：调用epoll_create(1024)创建一个 epoll 实例，1024表示初始监听的文件描述符数量
    epollfd_ = epoll_create(1024);
    this->UpdateChannel(wakeup_channel_);
    //添加唤醒通道：调用UpdateChannel方法，将 wakeup_channel_（用于唤醒任务调度器的通道）添加到 epoll 实例中
}

EpollTaskScheduler::~EpollTaskScheduler()
{
	
}

//添加或更新一个通道（Channel）到 epoll 实例中
void EpollTaskScheduler::UpdateChannel(ChannelPtr channel)
{
    std::cout << "线程标识2: " << std::this_thread::get_id() << std::endl;
    std::cout << "EpollTaskScheduler::UpdateChannel(ChannelPtr channel)" << std::endl;
    std::lock_guard<std::mutex> lock(mutex_);
    int fd = channel->GetSocket();
	if(channels_.find(fd) != channels_.end()) {
		if(channel->IsNoneEvent()) {
			Update(EPOLL_CTL_DEL, channel);
			channels_.erase(fd);
		}
		else {
			Update(EPOLL_CTL_MOD, channel);
		}
	}
	else {
		if(!channel->IsNoneEvent()) {
			channels_.emplace(fd, channel);
			Update(EPOLL_CTL_ADD, channel);
        }
    }
    /*
     * 检查通道是否存在：
    存在：
        如果通道没有注册任何事件（IsNoneEvent() 返回 true），
        则调用 Update 方法执行 EPOLL_CTL_DEL 操作，从 epoll 实例中删除通道，并从 channels_ 中移除。
        否则，调用 Update 方法执行 EPOLL_CTL_MOD 操作，修改通道的事件设置。
    不存在：
        如果通道注册了事件，则将通道添加到 channels_ 中，
        并调用 Update 方法执行 EPOLL_CTL_ADD 操作，将通道添加到 epoll 实例中
    */
}

//用于执行 epoll_ctl 操作（添加、修改或删除通道）
void EpollTaskScheduler::Update(int operation, ChannelPtr& channel)
{
	struct epoll_event event = {0};

    //如果操作不是 EPOLL_CTL_DEL（删除操作），则设置 event.data.ptr 为通道指针，event.events为通道注册的事件
    if (operation != EPOLL_CTL_DEL) {
        event.data.ptr = channel.get();
        event.events = channel->GetEvents();
    }

    //调用 epoll_ctl：执行 epoll_ctl 操作，参数包括 epoll 文件描述符、操作类型、套接字描述符和 epoll_event 结构体
    if (::epoll_ctl(epollfd_, operation, channel->GetSocket(), &event) < 0) {
    }
}

//从 epoll 实例中移除一个通道
void EpollTaskScheduler::RemoveChannel(ChannelPtr& channel)
{
    std::lock_guard<std::mutex> lock(mutex_);
	int fd = channel->GetSocket();

	if(channels_.find(fd) != channels_.end()) {
		Update(EPOLL_CTL_DEL, channel);
		channels_.erase(fd);
	}
    //检查通道是否存在：
    //如果存在，则调用 Update 方法执行 EPOLL_CTL_DEL 操作，从 epoll 实例中删除通道，并从 channels_ 中移除
}

//处理事件，timeout参数指定等待事件的最长时间（以毫秒为单位）。返回布尔值，表示是否成功处理事件。
bool EpollTaskScheduler::HandleEvent(int timeout)
{
    std::cout << "EpollTaskScheduler::HandleEvent(int timeout)" << std::endl;
    struct epoll_event events[512] = {0};
    int num_events = -1;

    //epoll_wait返回值，正数：表示发生的事件数量。0：表示超时。负数：表示发生错误
    num_events = epoll_wait(epollfd_, events, 512, timeout); //调用 epoll_wait：等待事件发生
    if (num_events < 0) {
        if (errno != EINTR) {
            return false;
        }
    }

    //处理事件：遍历所有发生的事件，调用对应 Channel 对象的 HandleEvent 方法处理事件
    for (int n = 0; n < num_events; n++) {
        if (events[n].data.ptr) {
            ((Channel*) events[n].data.ptr)->HandleEvent(events[n].events);
        }
    }
    return true; //返回 true，表示事件处理完成
}



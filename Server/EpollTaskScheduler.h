

#ifndef XOP_EPOLL_TASK_SCHEDULER_H
#define XOP_EPOLL_TASK_SCHEDULER_H

#include "TaskScheduler.h" //基类 TaskScheduler 的定义，提供了任务调度器的通用接口
#include <mutex>
#include <unordered_map>

class EpollTaskScheduler : public TaskScheduler
{
public:
	EpollTaskScheduler(int id = 0);
	virtual ~EpollTaskScheduler();

	void UpdateChannel(ChannelPtr channel);
	void RemoveChannel(ChannelPtr& channel);

	// timeout: ms
	bool HandleEvent(int timeout);

private:
	void Update(int operation, ChannelPtr& channel);

    int epollfd_ = -1; //epoll文件描述符，初始化为-1
    std::mutex mutex_;
    std::unordered_map<int, ChannelPtr> channels_;
    //无序映射,存储文件描述符与对应的 Channel 对象之间的关联
};

#endif

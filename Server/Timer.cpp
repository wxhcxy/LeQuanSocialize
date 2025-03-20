#include "Timer.h"
#include <iostream>

using namespace std;
using namespace std::chrono;

//添加定时器
TimerId TimerQueue::AddTimer(const TimerEvent& event, uint32_t ms)
{    
	std::lock_guard<std::mutex> locker(mutex_);

	int64_t timeout = GetTimeNow();
	TimerId timer_id = ++last_timer_id_;
    //生成 TimerId：使用 ++last_timer_id_ 生成唯一的 TimerId

    //创建Timer对象：使用make_shared 创建Timer 对象，传入事件回调函数event 和时间间隔ms
    auto timer = make_shared<Timer>(event, ms);
    //设置下一次触发时间：调用 SetNextTimeout 方法，将 next_timeout_ 设置为 timeout + ms
    timer->SetNextTimeout(timeout);
    timers_.emplace(timer_id, timer);
    events_.emplace(std::pair<int64_t, TimerId>(timeout + ms, timer_id), std::move(timer));
    return timer_id;
}

//移除定时器
void TimerQueue::RemoveTimer(TimerId timerId)
{
	std::lock_guard<std::mutex> locker(mutex_);

	auto iter = timers_.find(timerId);
	if (iter != timers_.end()) {
		int64_t timeout = iter->second->getNextTimeout();
		events_.erase(std::pair<int64_t, TimerId>(timeout, timerId));
		timers_.erase(timerId);
	}
}

//获取当前时间
int64_t TimerQueue::GetTimeNow()
{	
	auto time_point = steady_clock::now();	
	return duration_cast<milliseconds>(time_point.time_since_epoch()).count();
}

//获取剩余时间
int64_t TimerQueue::GetTimeRemaining()
{	
	std::lock_guard<std::mutex> locker(mutex_);

	if (timers_.empty()) {
		return -1;
	}

	int64_t msec = events_.begin()->first.first - GetTimeNow();
	if (msec < 0) {
		msec = 0;
	}

	return msec;
}

//处理定时器事件
void TimerQueue::HandleTimerEvent()
{
    std::cout << "TimerQueue::HandleTimerEvent()" << std::endl;
    if(!timers_.empty()) {
		std::lock_guard<std::mutex> locker(mutex_);
		int64_t timePoint = GetTimeNow();
		while(!timers_.empty() && events_.begin()->first.first<=timePoint)
		{	
			TimerId timerId = events_.begin()->first.second;
			bool flag = events_.begin()->second->event_callback_();
			if(flag == true) {
				events_.begin()->second->SetNextTimeout(timePoint);
				auto timerPtr = std::move(events_.begin()->second);
				events_.erase(events_.begin());
				events_.emplace(std::pair<int64_t, TimerId>(timerPtr->getNextTimeout(), timerId), timerPtr);
			}
			else {		
				events_.erase(events_.begin());
				timers_.erase(timerId);				
			}
		}	
	}
}


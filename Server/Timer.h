

#ifndef _XOP_TIMER_H
#define _XOP_TIMER_H

#include <map>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <cstdint>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>

//返回 true：表示定时器事件需要重复执行。返回 false：表示定时器事件只执行一次
typedef std::function<bool(void)> TimerEvent; //定时器事件回调函数，
typedef uint32_t TimerId;                     //唯一标识一个定时器

class Timer
{
public:
    //接受一个定时器事件回调函数event 和一个以毫秒为单位的时间间隔msec
    Timer(const TimerEvent& event, uint32_t msec)
        : event_callback_(event)
        , interval_(msec)
    {
        if (interval_ == 0) {
            interval_ = 1;
        }
    }

    //让当前线程休眠msec毫秒
    static void Sleep(uint32_t msec)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(msec));
    }

    //设置定时器事件回调函数event_callback_
    void SetEventCallback(const TimerEvent& event) { event_callback_ = event; }

    //启动定时器
    void Start(int64_t microseconds, bool repeat = false)
    {
        is_repeat_ = repeat; //指示定时器是否重复执行
        auto time_begin = std::chrono::high_resolution_clock::now();
        int64_t elapsed = 0;

        do {
            std::this_thread::sleep_for(std::chrono::microseconds(microseconds - elapsed));
            time_begin = std::chrono::high_resolution_clock::now(); //记录当前时间 time_begin
            if (event_callback_) {
                event_callback_();
            }
            elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::high_resolution_clock::now() - time_begin)
                          .count();
            if (elapsed < 0) {
                elapsed = 0;
            }

        } while (is_repeat_);
    }

    //停止定时器
    void Stop() { is_repeat_ = false; }

private:
    friend class TimerQueue;

    //设置下一次触发时间next_timeout_，即当前时间time_point 加上时间间隔interval_
    void SetNextTimeout(int64_t time_point) { next_timeout_ = time_point + interval_; }

    //返回下一次触发时间next_timeout_
    int64_t getNextTimeout() const { return next_timeout_; }

    bool is_repeat_ = false;
    //定时器事件回调函数，默认为一个不执行任何操作的空函数
    TimerEvent event_callback_ = [] { return false; };
    uint32_t interval_ = 0;    //定时器的时间间隔（以毫秒为单位）
    int64_t next_timeout_ = 0; //下一次触发时间
};

class TimerQueue
{
public:
    TimerId AddTimer(const TimerEvent& event, uint32_t msec); //添加一个定时器事件
    void RemoveTimer(TimerId timerId);                        //移除指定 TimerId 的定时器

    int64_t GetTimeRemaining(); //返回距离下一个定时器事件触发还有多少毫秒
    void HandleTimerEvent();    //理定时器事件，调用到期的定时器回调函数

private:
    int64_t GetTimeNow(); //获取当前时间点

    std::mutex mutex_;
    //无序映射，存储 TimerId 与 Timer 对象之间的关联
    std::unordered_map<TimerId, std::shared_ptr<Timer>> timers_;
    //有序映射，存储定时器事件，按照触发时间排序
    std::map<std::pair<int64_t, TimerId>, std::shared_ptr<Timer>> events_;

    uint32_t last_timer_id_ = 0; //用于生成唯一的 TimerId
};

#endif 




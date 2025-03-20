

#ifndef XOP_RING_BUFFER_H
#define XOP_RING_BUFFER_H

#include <atomic> //提供原子类型 std::atomic_int，用于线程安全的计数器
#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class RingBuffer
{
public:
    //参数capacity，用于设置缓冲区的大小。默认值为 60
    RingBuffer(int capacity = 60)
        : capacity_(capacity)
        , num_datas_(0)     //当前数据数量，初始化为 0
        , buffer_(capacity) //内部缓冲区，初始化为 capacity 大小的 std::vector<T>
    {}

    virtual ~RingBuffer() {}

    //添加数据到缓冲区
    //左值引用：接受左值引用 const T& data，调用 pushData 方法，并使用 std::forward 进行完美转发。
    bool Push(const T& data) { return pushData(std::forward<T>(data)); }

    //右值引用：接受右值引用 T&& data，调用 PushData 方法
    bool Push(T&& data) { return PushData(data); }

    //缓冲区中弹出数据到data 中
    bool Pop(T& data)
    {
        if (num_datas_ > 0) {
            data = std::move(buffer_[get_pos_]);
            Add(get_pos_);
            num_datas_--;
            return true;
        }

        return false;
    }

    //检查缓冲区是否已满
    bool IsFull() const { return ((num_datas_ == capacity_) ? true : false); }

    //检查缓冲区是否为空
    bool IsEmpty() const { return ((num_datas_ == 0) ? true : false); }

    //返回当前缓冲区中数据数量
    int Size() const { return num_datas_; }

private:
    template<typename F>
    bool PushData(F&& data) //用于添加数据到缓冲区
    {
        if (num_datas_ < capacity_) {
            buffer_[put_pos_] = std::forward<F>(data);
            Add(put_pos_);
            num_datas_++;
            return true;
        }

        return false;
    }

    //更新位置索引pos，实现环形缓冲区
    void Add(int& pos) { pos = (((pos + 1) == capacity_) ? 0 : (pos + 1)); }

    int capacity_ = 0; //缓冲区容量
    int put_pos_ = 0;  //下一个要写入数据的位置索引
    int get_pos_ = 0;  //下一个要读取数据的位置索引

    std::atomic_int num_datas_; //原子整数，用于线程安全地跟踪当前数据数量
    std::vector<T> buffer_;     //内部缓冲区，用于存储数据
};

#endif



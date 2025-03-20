
#ifndef XOP_BUFFER_WRITER_H
#define XOP_BUFFER_WRITER_H

#include <cstdint>
#include <memory>
#include <queue>
#include <string>
#include "Socket.h"

class BufferWriter
{
public:
    //设置内部队列的最大长度。默认值为 kMaxQueueLength（10000）
    BufferWriter(int capacity = kMaxQueueLength);
    ~BufferWriter() {}

    //将数据添加到内部缓冲区中
    bool Append(std::shared_ptr<char> data, uint32_t size, uint32_t index = 0);
    bool Append(const char* data, uint32_t size, uint32_t index = 0);
    int Send(int sockfd, int timeout = 0); //将缓冲区中的数据发送到套接字 sockfd

    //表示缓冲区是否为空
    bool IsEmpty() const { return buffer_.empty(); }

    //表示缓冲区是否已满
    bool IsFull() const { return ((int) buffer_.size() >= max_queue_length_ ? true : false); }

    //返回缓冲区中当前的数据包数量
    uint32_t Size() const { return (uint32_t) buffer_.size(); }

private:
    /*
     * 定义内部结构体 Packet，用于表示一个数据包：
     * data：指向数据的智能指针。
     * size：数据的大小。
     * writeIndex：当前写入的位置索引
    */
    typedef struct
    {
        std::shared_ptr<char> data;
        uint32_t size;
        uint32_t writeIndex;
    } Packet;

    std::queue<Packet> buffer_; //内部队列，用于存储待发送的数据包
    int max_queue_length_ = 0;  //队列的最大长度

    static const int kMaxQueueLength = 10000; //静态常量，表示队列的最大长度为 10000
};

#endif


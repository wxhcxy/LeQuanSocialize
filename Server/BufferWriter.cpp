

#include "BufferWriter.h"
#include "Socket.h"
#include "SocketUtil.h"


//设置内部队列的最大长度。默认值为 kMaxQueueLength（10000）
BufferWriter::BufferWriter(int capacity) 
	: max_queue_length_(capacity)
{}

bool BufferWriter::Append(std::shared_ptr<char> data, uint32_t size, uint32_t index)
{
    std::cout << "写缓冲区写入数据BufferWriter::Append shared_ptr: " << data << std::endl;
    //检查数据有效性：如果 size 小于等于 index，则返回false
    if (size <= index) {
        return false;
    }

    //检查队列容量：如果队列已满，则返回false
    if ((int) buffer_.size() >= max_queue_length_) {
        return false;
    }

    //创建数据包：构造 Packet 对象，将数据指针、大小和写入索引赋值给pkt
    Packet pkt = {data, size, index};
    buffer_.emplace(std::move(pkt)); //追加到队列：将 pkt 移动到队列中
    return true;
}

bool BufferWriter::Append(const char* data, uint32_t size, uint32_t index)
{
    //std::cout << "写缓冲区写入数据BufferWriter::Append char* data: " << data << std::endl;
    if (size <= index) { //检查数据有效性
        return false;
    }

    if ((int) buffer_.size() >= max_queue_length_) { //检查队列容量
        return false;
    }

    Packet pkt; //创建数据包
    pkt.data.reset(new char[size + 512], std::default_delete<char[]>());
    memcpy(pkt.data.get(), data, size);
    pkt.size = size;
    pkt.writeIndex = index;
    buffer_.emplace(std::move(pkt));
    return true;
}

//将缓冲区中的数据发送到套接字sockfd
int BufferWriter::Send(int sockfd, int timeout)
{
    //如果timeout大于0，则调用 SocketUtil::SetBlock 将套接字设置为阻塞模式，并设置超时时间
    if (timeout > 0) {
        SocketUtil::SetBlock(sockfd, timeout);
    }

    int ret = 0;
    int count = 1;

    do {
        if (buffer_.empty()) { //检查队列是否为空
            return 0;
        }

        count -= 1;
        Packet& pkt = buffer_.front(); //获取当前数据包，获取队列中的第一个数据包 pkt
        //发送数据：调用 ::send 发送数据，从 pkt.writeIndex 开始，发送 pkt.size - pkt.writeIndex 字节
        ret = ::send(sockfd, pkt.data.get() + pkt.writeIndex, pkt.size - pkt.writeIndex, 0);
        std::cout << "ret BufferWriter::Send: " << ret << std::endl;
        if (ret > 0) {
            pkt.writeIndex += ret;
            if (pkt.size == pkt.writeIndex) {
                count += 1;
                buffer_.pop();
            }
        } else if (ret < 0) {
            if (errno == EINTR || errno == EAGAIN)

            {
                std::cout << "出错" << std::endl;
                ret = 0;
            }
        }
    } while (count > 0);

    //恢复阻塞模式：如果 timeout 大于 0，则调用 SocketUtil::SetNonBlock 将套接字恢复为非阻塞模式
    if (timeout > 0) {
        SocketUtil::SetNonBlock(sockfd);
    }

    return ret;
}

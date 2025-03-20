

#ifndef XOP_BUFFER_READER_H
#define XOP_BUFFER_READER_H

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>  
#include <memory>  
#include "Socket.h"

class BufferReader
{
public:
    BufferReader(uint32_t initial_size = 2048); //默认缓冲区大小为 2048 字节
    virtual ~BufferReader();

    //返回缓冲区中可读取的字节数
    uint32_t ReadableBytes() const { return (uint32_t) (writer_index_ - reader_index_); }

    //返回缓冲区中可写入的字节数
    uint32_t WritableBytes() const { return (uint32_t) (buffer_.size() - writer_index_); }

    //返回指向缓冲区中当前读取位置的指针。const 版本用于常量对象
    char* Peek() { return Begin() + reader_index_; }

    const char* Peek() const { return Begin() + reader_index_; }

    //查找缓冲区中第一个 CRLF（\r\n）的位置
    const char* FindFirstCrlf() const
    {
        const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
        return crlf == BeginWrite() ? nullptr : crlf;
        //使用 std::search 在 Peek() 和 BeginWrite() 之间搜索 CRLF
        //如果找到，返回指向 CRLF 的指针
    }

    //查找缓冲区中最后一个 CRLF 的位置
    const char* FindLastCrlf() const
    {
        const char* crlf = std::find_end(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
        return crlf == BeginWrite() ? nullptr : crlf;
    }

    //查找缓冲区中最后一个 CRLF CRLF（\r\n\r\n）的位置
    //常用于解析 HTTP 头部，标识头部结束的位置
    const char* FindLastCrlfCrlf() const
    {
        char crlfCrlf[] = "\r\n\r\n";
        const char* crlf = std::find_end(Peek(), BeginWrite(), crlfCrlf, crlfCrlf + 4);
        return crlf == BeginWrite() ? nullptr : crlf;
    }

    //清空缓冲区，重置读写指针
    void RetrieveAll()
    {
        writer_index_ = 0;
        reader_index_ = 0;
    }

    //消费缓冲区中的数据，参数len 表示要消费的字节数
    void Retrieve(size_t len)
    {
        if (len <= ReadableBytes()) {
            reader_index_ += len;
            if (reader_index_ == writer_index_) {
                reader_index_ = 0;
                writer_index_ = 0;
            }
        } else {
            RetrieveAll();
        }
        /*
         *如果 len 小于等于可读取的字节数，则将 reader_index_ 增加 len。
         *如果 reader_index_ 等于 writer_index_，则重置读写指针。
         *否则，调用 RetrieveAll() 清空缓冲区。
        */
    }

    //消费缓冲区中的数据，直到指定的位置end
    //计算end 与当前读取位置 Peek() 之间的字节数，并调用 Retrieve 方法。
    void RetrieveUntil(const char* end) { Retrieve(end - Peek()); }

    int Read(int sockfd);
    uint32_t ReadAll(std::string& data);
    uint32_t ReadUntilCrlf(std::string& data);

    //返回缓冲区的大小
    uint32_t Size() const { return (uint32_t) buffer_.size(); }

private:
    //返回指向缓冲区起始位置的指针，const 版本用于常量对象
    char* Begin() { return &*buffer_.begin(); }

    const char* Begin() const { return &*buffer_.begin(); }

    //返回指向缓冲区中当前写入位置的指针，const 版本用于常量对象
    char* beginWrite() { return Begin() + writer_index_; }

    const char* BeginWrite() const { return Begin() + writer_index_; }

    std::vector<char> buffer_; //内部缓冲区，用于存储接收到的数据
    size_t reader_index_ = 0;  //读取指针，表示下一个要读取的字节位置
    size_t writer_index_ = 0;  //写入指针，表示下一个要写入的字节位置

    static const char kCRLF[];                       //静态常量，表示 CRLF 字符串
    static const uint32_t MAX_BYTES_PER_READ = 4096; //每次从套接字读取的最大字节数，默认为 4096 字节
    static const uint32_t MAX_BUFFER_SIZE = 1024 * 100000; //缓冲区最大大小,默认为10240000字节10MB)
};

#endif



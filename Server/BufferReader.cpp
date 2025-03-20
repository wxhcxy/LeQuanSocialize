

#include "BufferReader.h"
#include "Socket.h"
#include <boost/json.hpp>
#include <cstring>
#include <thread>

const char BufferReader::kCRLF[] = "\r\n";

//参数initial_size，用于初始化内部缓冲区的大小
BufferReader::BufferReader(uint32_t initial_size)
{
    std::cout << "1buffer_ vector size(): " << buffer_.size() << std::endl;
    buffer_.resize(initial_size);
    std::cout << "2buffer_ vector size(): " << buffer_.size() << std::endl;
}

BufferReader::~BufferReader()
{
	
}

//Read函数负责接收客户端发送的数据，并通过beginWrite(),写入buffer_缓冲区
//从套接字sockfd 读取数据到缓冲区
int BufferReader::Read(int sockfd)
{
    std::cout << "!!!BufferReader::Read(int sockfd)线程标识: " << std::this_thread::get_id()
              << std::endl;
    std::cout << "3buffer_ vector size(): " << buffer_.size() << std::endl;

    std::cout << "读缓冲区开始从套接字读取数据" << std::endl;
    uint32_t size = WritableBytes();
    if (size < MAX_BYTES_PER_READ) {
        uint32_t bufferReaderSize = (uint32_t) buffer_.size();
        if (bufferReaderSize > MAX_BUFFER_SIZE) {
            return 0;
        }

        buffer_.resize(bufferReaderSize + MAX_BYTES_PER_READ);
    }

    int bytes_read = ::recv(sockfd, beginWrite(), MAX_BYTES_PER_READ, 0);

    if (bytes_read > 0) {
        writer_index_ += bytes_read;
    } else if (bytes_read == 0) {
        std::cout << "套接字已关闭" << std::endl;
    }

    //std::string strTest;
    //ReadAll(strTest);
    //std::cout << "BufferReader接收到的信息：" << strTest << std::endl;

    // std::cout << "4buffer_ vector size(): " << buffer_.size() << std::endl;

    // //测试输出从套接字读取到的数据
    // std::string str;
    // ReadAll(str);
    // std::cout << str << std::endl;
    // // 解析 JSON 字符串
    // boost::json::value jv = boost::json::parse(str);
    // std::string id = jv.as_object()["id"].as_string().c_str();
    // std::string name = jv.as_object()["name"].as_string().c_str();
    // std::cout << "id: " << id << std::endl;
    // std::cout << "name: " << name << std::endl;

    return bytes_read;
}

//ReadAll函数负责将缓冲区中的数据输出到参数std::string& data里
//读取所有数据
//读取缓冲区中所有可读的数据到 std::string 对象data 中
uint32_t BufferReader::ReadAll(std::string& data)
{
    // uint32_t size = ReadableBytes();
    // if (size > 0) {
    //     data.assign(Peek(), size); //如果可读取字节数大于 0，则将数据赋值给 data
    //     writer_index_ = 0;         //重置读写指针
    //     reader_index_ = 0;
    // }

    uint32_t size = ReadableBytes();
    std::cout << "size: " << size << std::endl;
    if (size >= 4) { // 假设前4个字节是长度，处理粘包
        uint32_t data_length;
        memcpy(&data_length, Peek(), sizeof(uint32_t));
        data_length = ntohl(
            data_length); // 转换字节序,如果客户端发送消息时转换了字节序，这里就也要转换
        //std::cout << "解析到的数据长度: " << data_length << std::endl;

        if (size >= 4 + data_length) {
            data.assign(Peek() + 4, data_length);
            Retrieve(4 + data_length);
            return data_length;
        }
    }

    return size;
}

//读取缓冲区中直到第一个 CRLF（\r\n） 的数据到 std::string 对象data 中
uint32_t BufferReader::ReadUntilCrlf(std::string& data)
{
	const char* crlf = FindLastCrlf();
	if(crlf == nullptr)  {
		return 0;
	}

	uint32_t size = (uint32_t)(crlf - Peek() + 2);
	data.assign(Peek(), size);
	Retrieve(size);
	return size;
    //将数据赋值给 data，并调用 Retrieve 方法消费缓冲区中的数据
}


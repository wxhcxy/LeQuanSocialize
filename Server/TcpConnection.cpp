#include "TcpConnection.h"
#include "SocketUtil.h"
#include <boost/json.hpp>

#include "controllerfactory.h"

//接受一个指向 TaskScheduler 的指针和套接字描述符sockfd，用于初始化连接。
TcpConnection::TcpConnection(TaskScheduler *task_scheduler, int sockfd)
    : task_scheduler_(task_scheduler)
    , read_buffer_(new BufferReader)       //创建对象，用于读取数据
    , write_buffer_(new BufferWriter(500)) //初始化缓冲区大小为500字节，用于写入数据
    , channel_(new Channel(sockfd)) //创建 Channel 对象，关联到套接字描述符 sockfd
{
    is_closed_ = false; //表示连接未关闭

    //设置 channel_ 的读取、写入、关闭和错误回调函数，指向 HandleRead、HandleWrite、HandleClose 和 HandleError 方法
    channel_->SetReadCallback([this]() { this->HandleRead(); });
    channel_->SetWriteCallback([this]() { this->HandleWrite(); });
    channel_->SetCloseCallback([this]() { this->HandleClose(); });
    channel_->SetErrorCallback([this]() { this->HandleError(); });

    SocketUtil::SetNonBlock(sockfd);                //将套接字设置为非阻塞模式
    SocketUtil::SetSendBufSize(sockfd, 100 * 1024); //设置发送缓冲区大小为 100KB
    SocketUtil::SetKeepAlive(sockfd);               //启用 TCP 保活机制

    channel_->EnableReading(); //启用通道的读取功能
    task_scheduler_->UpdateChannel(channel_); //更新通道，将通道添加到任务调度器中，开始监听事件

    //测试，可能TcpConnection里不应用关联控制层
    m_scanAndCheckJottingController = ControllerFactory::getInstance()
                                          ->createScanAndCheckJottingController();
    m_initController = ControllerFactory::getInstance()->createInitController();
    m_messageController = ControllerFactory::getInstance()->createMessageController();
    m_publishJottingController = ControllerFactory::getInstance()->createPublishJottingController();
    m_cacheFlush = ControllerFactory::getInstance()->createCacheFlush();
}

TcpConnection::~TcpConnection()
{
    int fd = channel_->GetSocket();
    if (fd > 0) {
		SocketUtil::Close(fd);
	}
}

//发送数据
void TcpConnection::Send(std::shared_ptr<char> data, uint32_t size)
{
	if (!is_closed_) {
		mutex_.lock();
        write_buffer_->Append(data, size); //数据追加到 write_buffer_
        mutex_.unlock();

        this->HandleWrite(); //处理写入事件
    }
}

void TcpConnection::Send(const char *data, uint32_t size)
{
    //std::cout << "服务器准备发送: " << data << std::endl;
    std::cout << "服务器准备发送size: " << size << std::endl;
    std::cout << "is_closed_: " << is_closed_ << std::endl;
    if (!is_closed_) {
        std::cout << "进入写缓冲区" << std::endl;
        // 序列化消息头部长度为网络字节序
        uint32_t network_size = htonl(size);

        // 先发送消息头部长度
        write_buffer_->Append(reinterpret_cast<char *>(&network_size), sizeof(network_size));

        // 再发送实际数据
        write_buffer_->Append(data, size);

        // 发送数据
        this->HandleWrite();
    }

    // std::cout << "服务器准备发送: " << data << std::endl;
    // std::cout << "is_closed_: " << is_closed_ << std::endl;
    // if (!is_closed_) {
    //     std::cout << "进入写缓冲区" << std::endl;
    //     //mutex_.lock();//为什么这里不注释代码就不会运行到这里？
    //     write_buffer_->Append(data, size);
    //     //mutex_.unlock();

    //     std::cout << "服务器发送消息TcpConnection::Send：" << std::endl;
    //     this->HandleWrite();
    // }
}

void TcpConnection::Disconnect()
{
    //获取自身的智能指针 conn
    //调用 task_scheduler_->AddTriggerEvent 方法，添加一个触发事件，用于调用 Close() 方法。
    std::lock_guard<std::mutex> lock(mutex_);
    auto conn = shared_from_this();
    task_scheduler_->AddTriggerEvent([conn]() { conn->Close(); });
}

//处理读取事件
void TcpConnection::HandleRead()
{
    std::cout << "!!!TcpConnection::HandleRead()线程标识: " << std::this_thread::get_id()
              << std::endl;
    std::cout << "连接处理读TcpConnection::HandleRead()" << std::endl;
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (is_closed_) {
			return;
		}

        //从套接字读取数据
        int ret = read_buffer_->Read(channel_->GetSocket());
        if (ret <= 0) {
            this->Close();
            return;
        }

        //测试读取数据
        std::string strTest;
        read_buffer_->ReadAll(strTest);
        std::cout << "服务器接收到的信息：" << strTest << std::endl;

        //处理客户端发来的不同的请求
        boost::json::error_code ec;
        boost::json::object message = boost::json::parse(strTest.c_str(), ec).as_object();

        if (ec) {
            std::cerr << "JSON parse error: " << ec.message() << std::endl;
            return;
        }

        // 检查 JSON 对象是否为空
        if (message.empty()) {
            std::cout << "Server receive nullptr" << std::endl;
            return;
        }

        // 获取 "request" 和 "id" 字段
        std::string request = message["request"].as_string().c_str();
        std::string netizenId = message["id"].as_string().c_str();

        std::cout << "Server receive request: " << request << std::endl;

        if (request == "ScanJottings") {
            std::cout << "    ----Netizen " << netizenId << " : ScanJottings----    " << std::endl;
            // 调用 pushJottings 方法获取要推送的笔记信息
            boost::json::object j = m_scanAndCheckJottingController->pushJottings(netizenId);
            std::cout << "笔记推送加载完成" << std::endl;
            // 序列化 JSON 对象为字符串
            std::string s = boost::json::serialize(j);
            // //std::cout << s << std::endl;
            // std::cout << "网民id，netizenId: " << j["netizenId"] << std::endl;
            // std::cout << "网民昵称，nickName: " << j["nickName"] << std::endl;
            // 发送 JSON 字符串
            // std::string testStr = "qwerewqewqewerwqewassw";
            // Send(testStr.data(), testStr.size());
            Send(s.data(), s.size());
            //network.sendMessage(s.data(), s.size());
            // 输出结束信息
            std::cout << "----ScanJottings ends----     " << std::endl;
        } else if (request == "InitPersonalInfo") {
            std::cout << "    ----Netizen " << netizenId << " : InitPersonalInfo----    "
                      << std::endl;
            boost::json::object j = m_initController->getInfo(netizenId);
            std::string s = boost::json::serialize(j);
            //std::cout << s << std::endl;
            //            send(s.data(),s.size(),fd);
            //network.sendMessage(s.data(), s.size());
            Send(s.data(), s.size());
            std::cout << "    ----Init final----  " << std::endl;
        } else if (request == "GetJottingDetail") {
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << "获取笔记详细信息" << std::endl;
            std::string jottingId = message["jottingId"].as_string().c_str();
            std::cout << "    ----Netizen " << netizenId << " GetJottingDetail " << jottingId
                      << "----    " << std::endl;
            boost::json::object j = m_scanAndCheckJottingController->pushJottingDetial(netizenId,
                                                                                       jottingId);
            std::string s = boost::json::serialize(j);
            //std::cout << s << std::endl;
            //            send(s.data(),s.size(),fd);
            //network.sendMessage(s.data(), s.size());
            Send(s.data(), s.size());
            std::cout << "   ---- GetJottingDetail end----    " << std::endl;
        } else if (request == "Comment") {
            std::cout << "    ----Netizen " << netizenId << " : Receive Comment---- " << std::endl;
            std::string comment = message["text"].as_string().c_str();
            std::string jottingId = message["jottingId"].as_string().c_str();
            std::cout << "jottingId:" << jottingId << "\n";
            std::cout << "comment:" << comment << "\n";
            m_publishJottingController->comment(netizenId, comment, jottingId);
            std::cout << "   ----Receive end----" << std::endl;
        } else if (request == "GetMessage") {
            std::cout << "    ----Netizen " << netizenId << " : GetMessage----    " << std::endl;
            boost::json::object j = m_messageController->getJottingNotification(netizenId);
            std::string s = boost::json::serialize(j);
            Send(s.data(), s.size());
            std::cout << s << std::endl;
            //network.sendMessage(s.data(), s.size());
            //            send(s.data(),s.size(),fd);
            std::cout << "   ---- GetMessage end----    " << std::endl;
        } else if (request == "GetInfoJottingDetail") { //个人主页中的笔记详情
            std::string jottingId = message["jottingId"].as_string().c_str();
            std::cout << "    ----Netizen " << netizenId << " : GetInfoJottingDetail----    "
                      << jottingId << std::endl;
            boost::json::object j = m_scanAndCheckJottingController
                                        ->pushInfoJottingDetail(netizenId, jottingId);
            std::string s = boost::json::serialize(j);
            Send(s.data(), s.size());
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << "服务器收到点击个人笔记!!!!!!!" << std::endl;
            //std::cout << s << std::endl;
            //network.sendMessage(s.data(), s.size());
            //            send(s.data(),s.size(),fd);
            std::cout << "   ---- GetInfoJottingDetail end----    " << std::endl;
        } else if (request == "ScanVideos") {
            std::cout << "    ----Netizen " << netizenId << " : ScanVideos----    " << std::endl;
            boost::json::object j = m_scanAndCheckJottingController->scanVideos(netizenId);
            std::string s = boost::json::serialize(j);
            Send(s.data(), s.size());
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << "服务器收到浏览视频!!!!!!!" << std::endl;
            //std::cout << s << std::endl;
            //network.sendMessage(s.data(), s.size());
            std::cout << "   ---- ScanVideos end----    " << std::endl;
        } else if (request == "PublishJotting") {
            std::cout << "    ----Netizen " << netizenId << " : PublishJotting----    "
                      << std::endl;
            std::string isPub = m_publishJottingController
                                    ->publishJottings(netizenId, message["jotting"].as_object());
            Send(isPub.data(), isPub.size());
            //std::cout << isPub << std::endl;
            //network.sendMessage(isPub.data(), isPub.size());
            //            send(isPub.data(),isPub.size(),fd);
            std::cout << "Server << 接收笔记信息 发布成功!" << std::endl;
        }
    }

    //调用回调函数，传递 TcpConnection 对象和 BufferReader 对象作为参数
    //这下面的代码在程序中好像没有运行，因为read_cb_没有赋值函数对象
    // if (read_cb_) {
    //     std::cout << "read_cb_(shared_from_this(), *read_buffer_) TcpConnection::HandleRead() "
    //               << std::endl;
    //     bool ret = read_cb_(shared_from_this(), *read_buffer_);
    //     if (false == ret) {
    //         std::lock_guard<std::mutex> lock(mutex_);
    //         this->Close();
    //     }
    // }
}

void TcpConnection::HandleWrite()
{
    std::cout << "连接处理写TcpConnection::HandleWrite()" << std::endl;
    if (is_closed_) {
        return;
    }

    //这里互斥锁不注释也直接就返回了
    //std::lock_guard<std::mutex> lock(mutex_);
    // if (!mutex_.try_lock()) { //尝试锁定互斥锁。如果锁定失败，则返回，避免死锁
    //     return;
    // }

    int ret = 0;
    bool empty = false;
    do {
        std::cout << "发送数据TcpConnection::HandleWrite()" << std::endl;
        ret = write_buffer_->Send(channel_->GetSocket()); //发送数据
        std::cout << "ret: " << ret << std::endl;
        if (ret < 0) {
            this->Close();
            mutex_.unlock();
            return;
        }
        empty = write_buffer_->IsEmpty();
    } while (0);

    //如果发送缓冲区为空，则禁用写入事件；否则，启用写入事件，并更新通道状态
    if (empty) {
        if (channel_->IsWriting()) {
            channel_->DisableWriting();
            task_scheduler_->UpdateChannel(channel_);
        }
    } else if (!channel_->IsWriting()) {
        channel_->EnableWriting();
        task_scheduler_->UpdateChannel(channel_);
    }

    mutex_.unlock();
}

void TcpConnection::Close()
{
	if (!is_closed_) {
		is_closed_ = true;
        task_scheduler_->RemoveChannel(channel_); //移除通道，停止监听事件

        //调用 close_cb_ 和 disconnect_cb_ 回调函数，通知上层代码连接已关闭
        if (close_cb_) {
            close_cb_(shared_from_this());
        }

        if (disconnect_cb_) {
            disconnect_cb_(shared_from_this());
        }
    }
}

void TcpConnection::HandleClose()
{
	std::lock_guard<std::mutex> lock(mutex_);
	this->Close();
}

//处理错误事件
void TcpConnection::HandleError()
{
	std::lock_guard<std::mutex> lock(mutex_);
	this->Close();
}

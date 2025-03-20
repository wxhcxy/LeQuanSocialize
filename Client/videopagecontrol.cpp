#include "videopagecontrol.h"


#include "client.h"
#include "clientInfo.h"
#include "base64.h"
#include "imageprovider.h"

#include <boost/json.hpp>
#include <iostream>
#include <ostream>

extern ImageProvider *imageProvider;
VideoPageControl::VideoPageControl(QObject *parent):
    QObject(parent)
{

}

void VideoPageControl::pushRecommendVideos()
{
    std::cout << "进入函数 pushRecommendVideos" << std::endl;

    // 创建 JSON 消息对象
    boost::json::object message;
    message["id"] = ID;
    message["request"] = "ScanVideos";

    // 序列化 JSON 对象为字符串
    std::string sendData = boost::json::serialize(message);
    std::cout << "发送数据: " << sendData << std::endl;

    // 发送数据
    Client::getInstance()->reconnect();
    Client::getInstance()->send(sendData.c_str(), sendData.length());
    std::cout << "客户端获取用户推送视频的信息......" << std::endl;

    // 接收数据
    char *receiveData = new char[10000000];
    Client::getInstance()->receive(receiveData);
    std::cout << "接收到的数据: " << receiveData << std::endl;

    // 解析 JSON 字符串
    boost::json::error_code ec;
    boost::json::value parsed = boost::json::parse(std::string(receiveData), ec);
    delete[] receiveData;

    if (ec) {
        std::cout << "JSON 解析错误: " << ec.message() << std::endl;
        return;
    }

    // 获取 "videos" 数组
    boost::json::object scanInfo = parsed.as_object();
    boost::json::array videosArray = scanInfo["videos"].as_array();
    std::cout << "解析后的 JSON:" << std::endl << boost::json::serialize(scanInfo) << std::endl;

    // 初始化接收到的视频对象
    boost::json::object receiveVideos;
    receiveVideos["videos"] = boost::json::array();

    // 初始化头像哈希表
    QHash<QString, QPixmap> avatars;
    QHash<QString, QPixmap> allPics;

    // 遍历每个视频信息
    for (const auto &info : videosArray) {
        boost::json::object videoInfo;

        // 设置视频信息字段
        videoInfo["nickName"] = info.at("name").as_string().c_str();
        videoInfo["content"] = info.at("content").as_string().c_str();
        videoInfo["path"] = info.at("path").as_string().c_str();

        // 处理头像信息
        std::string avatarId = info.at("avatarId").as_string().c_str();
        std::string avatarPath = "image://photos/" + avatarId;
        std::string avatarData = base64_decode(info.at("avatar").as_string().c_str());

        // 加载 QPixmap
        QPixmap pixmap;
        pixmap.loadFromData(reinterpret_cast<const unsigned char *>(avatarData.c_str()),
                            static_cast<int>(avatarData.length()));
        avatars.insert(QString::fromStdString(avatarId), pixmap);
        videoInfo["avatar"] = avatarPath;

        // 添加到接收到的视频数组中
        receiveVideos["videos"].as_array().emplace_back(videoInfo);
    }

    // 设置头像哈希表
    imageProvider->setVideoAvatars(avatars);

    // 序列化接收到的视频对象为字符串
    std::string serializedVideos = boost::json::serialize(receiveVideos);
    std::cout << "接收到的视频信息:" << std::endl << serializedVideos << std::endl;

    // 转换为 QString
    m_videos = QString::fromStdString(serializedVideos);

    // 输出视频个数
    std::cout << "视频个数：" << receiveVideos["videos"].as_array().size() << std::endl;

    // 关闭套接字
    Client::getInstance()->closeSocket();
}

void VideoPageControl::setNickName(const QString &nickName)
{
    m_nickName=nickName;
}

QString VideoPageControl::getNickName() const
{
    return m_nickName;
}


void VideoPageControl::setAvatar(const QString &avatar)
{
    m_avatar=avatar;
    emit avatarChanged(m_avatar);
}

QString VideoPageControl::getAvatar() const
{
    return m_avatar;
}

void VideoPageControl::setVideos(const QString &videos)
{
    m_videos = videos;
}

QString VideoPageControl::getVideos() const
{
    return m_videos;
}

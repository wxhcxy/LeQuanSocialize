#include "messageuicontrol.h"


#include "base64.h"
#include "client.h"
#include "clientInfo.h"
#include "imageprovider.h"
#include "qpixmap.h"

#include <boost/json.hpp>
#include <iostream>

using namespace std;

extern ImageProvider *imageProvider;


MessageUIControl::MessageUIControl(QObject *parent):QObject(parent)
{

}

void MessageUIControl::scanMessage()
{
    std::cout << "进入函数 scanMessage" << std::endl;

    // 创建 JSON 消息对象
    boost::json::object message;
    message["id"] = ID;
    message["request"] = "GetMessage";

    // 序列化 JSON 对象为字符串
    std::string sendData = boost::json::serialize(message);
    std::cout << "发送数据: " << sendData << std::endl;

    // 发送数据
    Client::getInstance()->reconnect();
    Client::getInstance()->send(sendData.c_str(), sendData.length());
    std::cout << "客户端获取消息通知......" << std::endl;

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

    // 获取 "notification" 数组
    boost::json::object mes = parsed.as_object();
    mes["notification"] = boost::json::array();
    boost::json::array notifications = mes["notification"].as_array();
    std::cout << "解析后的 JSON:" << std::endl << boost::json::serialize(mes) << std::endl;

    // 初始化接收到的通知对象
    boost::json::object receiveNotifications;
    receiveNotifications["notification"] = boost::json::array();

    // 初始化头像哈希表
    QHash<QString, QPixmap> avatars;

    // 遍历每个通知
    for (const auto &info : notifications) {
        boost::json::object oneNotification;

        // 获取笔记作者头像信息
        std::string avatarId = info.at("netizen").at("avatarId").as_string().c_str();
        std::string avatarPath = "image://photos/" + avatarId;
        std::string avatarData = base64_decode(info.at("netizen").at("avatar").as_string().c_str());

        // 加载 QPixmap
        QPixmap pixmap;
        pixmap.loadFromData(reinterpret_cast<const unsigned char *>(avatarData.c_str()),
                            static_cast<int>(avatarData.length()));
        avatars.insert(QString::fromStdString(avatarId), pixmap);
        oneNotification["avatar"] = avatarPath;

        // 设置昵称
        oneNotification["nickName"] = info.at("netizen").at("nickName").as_string().c_str();

        // 设置内容
        std::string str = info.at("netizen").at("nickName").as_string().c_str();
        std::string content = " 您关注的 " + str + " 发布了新的笔记！";
        oneNotification["content"] = content;

        // 设置 jottingId 和时间
        oneNotification["jottingId"] = info.at("jottingid").as_string().c_str();
        oneNotification["time"] = info.at("time").as_string().c_str();

        // 添加到接收到的通知数组中
        receiveNotifications["notification"].as_array().emplace_back(oneNotification);
    }

    // 设置头像哈希表
    imageProvider->setMessageUIAvatars(avatars);

    // 序列化接收到的通知对象为字符串
    std::string serializedNotifications = boost::json::serialize(receiveNotifications);
    std::cout << "接收到的通知:" << std::endl << serializedNotifications << std::endl;

    // 转换为 QString
    m_notifications = QString::fromStdString(serializedNotifications);
}

void MessageUIControl::setNotifications(const QString &notifications)
{
    m_notifications = notifications;
}

QString MessageUIControl::getNotifications() const
{
    return m_notifications;
}

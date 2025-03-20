#include "messagecontroller.h"
#include "jottingbroker.h"
#include "messagesequence.h"
#include "netizen.h"
#include "netizenbroker.h"

MessageController* MessageController::m_instance = nullptr;
MessageController *MessageController::getInstance()
{
    if(m_instance==nullptr){
        m_instance = new MessageController();
    }
    return m_instance;
}

boost::json::object MessageController::getJottingNotification(std::string netizenId)
{
    Netizen& netizen = NetizenBroker::getInstance()->findById(netizenId);
    // 更新此人所拥有的消息
    NotificationFlush(netizenId);
    boost::json::object messages;

    // 获取笔记通知的 ID 列表
    std::vector<std::string> jottingId = netizen.getJottingNotification();

    // 初始化 "messages" 为一个数组
    messages["messages"] = boost::json::array();

    // 遍历每个 jottingId 并添加相应的消息摘要
    for (const auto& item : jottingId) {
        Jotting& jot = JottingBroker::getInstance()->findById(item);
        messages["messages"].as_array().emplace_back(jot.getMessageAbstract());
    }

    return messages;
}

void MessageController::NotificationFlush(std::string netizenId)
{
//    std::cout<<"消息推送"<<std::endl;
    std::cout<<"更新这个人所拥有的消息"<<std::endl;
    MessageSequence::getInstance()->updateMessageQueue(netizenId);
}

MessageController::MessageController()
{
//    m_myTimer.start(30000,NotificationFlush);
}

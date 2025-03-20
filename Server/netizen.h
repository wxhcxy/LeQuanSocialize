#ifndef NETIZEN_H
#define NETIZEN_H

#include "commentproxy.h"
#include "jottingproxy.h"
#include "netizeninterface.h"
#include "netizenproxy.h"
#include <boost/json.hpp>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class Netizen : public NetizenInterface
{
public:
    explicit Netizen(const std::string& tid);
    explicit Netizen(const std::string id,
                     std::string nickName,
                     std::string signal,
                     std::string avatar,
                     std::vector<std::string> jottingId,
                     std::vector<std::string> fansId,
                     std::vector<std::string> conceredsId,
                     std::vector<std::string> commentdId);

    virtual ~Netizen() {}

    boost::json::object scanJottings();

    virtual boost::json::object getAbstract() override;
    const std::string readLog();
    void writeLog();

    boost::json::object getInfo();
    boost::json::array getFansInfo();
    boost::json::array getConcernedInfo();
    //首页中的笔记详情是不需要再次发送图片的
    boost::json::object checkOneJotting(std::string jottingId);
    void updateMessage(std::string jottingId) override; //更新消息关联
    std::vector<std::string> getJottingNotification();
    bool comment(const std::string content, const std::string jottingId);
    //个人主页中的笔记详情是需要再次发送图片的
    boost::json::object checkInfoOneJotting(std::string jottingId);
    boost::json::object scanVideos();
    bool publishJotting(boost::json::object jotting_json);

private:
    std::string m_nickName;
    std::string m_signal;
    std::string m_avatar;

    std::unordered_map<std::string, JottingProxy> _jottings;
    std::unordered_map<std::string, NetizenProxy> _fans;
    std::unordered_map<std::string, NetizenProxy> _concerneds;
    std::unordered_map<std::string, CommentProxy> _comments;

    //存放的内容为通知的笔记的id
    std::set<std::string> _messages;
    bool m_online;

    std::string m_id;
};

#endif // NETIZEN_H

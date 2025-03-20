#ifndef JOTTINGINTERFACE_H
#define JOTTINGINTERFACE_H

#include <boost/json.hpp>
#include <string>
#include <unordered_map>

class JottingInterface
{
public:
    explicit JottingInterface(const std::string &tid);
    virtual ~JottingInterface() {}

    std::string id() const;

    virtual boost::json::object getAbstract() = 0;
    virtual boost::json::object getOnePicAbstract() = 0;
    virtual boost::json::object getDetail() = 0;          //主页的笔记详情
    virtual boost::json::object getMessageAbstract() = 0; //消息界面
    virtual boost::json::object getInfoDetail() = 0;      //非主页的笔记详情

private:
    std::string m_timeId;
};

#endif // JOTTINGINTERFACE_H

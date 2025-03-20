#ifndef JOTTINGPROXY_H
#define JOTTINGPROXY_H

#include "jottinginterface.h"

#include <memory>

class Jotting;

class JottingProxy : public JottingInterface
{
public:
    explicit JottingProxy(const std::string &tid);
    virtual ~JottingProxy();

    virtual boost::json::object getAbstract() override;
    virtual boost::json::object getOnePicAbstract() override;
    virtual boost::json::object getDetail() override;
    virtual boost::json::object getMessageAbstract();     //消息界面
    virtual boost::json::object getInfoDetail() override; //非主页的笔记详情

private:
    Jotting *_jotting;
};

#endif // JOTTINGPROXY_H

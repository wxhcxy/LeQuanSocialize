#include "jottingproxy.h"
#include "jotting.h"
#include "jottingbroker.h"

JottingProxy::JottingProxy(const std::string &tid)
    : JottingInterface{tid}
    , _jotting(nullptr)
{}

JottingProxy::~JottingProxy() {}

boost::json::object JottingProxy::getAbstract()
{
    if (_jotting == nullptr) {
        _jotting = &JottingBroker::getInstance()->findById(id());
    }
    return _jotting->getAbstract();
}

boost::json::object JottingProxy::getOnePicAbstract()
{
    try {
        if (_jotting == nullptr) {
            _jotting = &JottingBroker::getInstance()->findById(id());
        }
        return _jotting->getOnePicAbstract();
    } catch (...) {
        std::cout << "get One Pic Abstract error";
    }
}

boost::json::object JottingProxy::getDetail()
{
    if (_jotting == nullptr) {
        _jotting = &JottingBroker::getInstance()->findById(id());
    }
    return _jotting->getDetail();
}

boost::json::object JottingProxy::getMessageAbstract()
{
    if (_jotting == nullptr) {
        _jotting = &JottingBroker::getInstance()->findById(id());
    }
    return _jotting->getMessageAbstract();
}

boost::json::object JottingProxy::getInfoDetail()
{
    if (_jotting == nullptr) {
        _jotting = &JottingBroker::getInstance()->findById(id());
    }
    return _jotting->getInfoDetail();
}

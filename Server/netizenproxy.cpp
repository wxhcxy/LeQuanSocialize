#include "netizenproxy.h"
#include "netizen.h"
#include "netizenbroker.h"

NetizenProxy::NetizenProxy(const std::string &tid)
    : NetizenInterface(tid)
    , _netizen(nullptr)
{}

NetizenProxy::~NetizenProxy() {}

boost::json::object NetizenProxy::getAbstract()
{
    if (_netizen == nullptr) {
        _netizen = &NetizenBroker::getInstance()->findById(id());
    }
    return _netizen->getAbstract();
}

void NetizenProxy::updateMessage(std::string jottingId)
{
    if (_netizen == nullptr) {
        _netizen = &NetizenBroker::getInstance()->findById(id());
    }
    return _netizen->updateMessage(jottingId);
}

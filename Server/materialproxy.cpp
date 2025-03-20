#include "materialproxy.h"
#include "material.h"
#include "materialbroker.h"

MaterialProxy::MaterialProxy(const std::string &tid)
    : MaterialInterface(tid)
    , _material(nullptr)
{}

MaterialProxy::~MaterialProxy() {}

boost::json::object MaterialProxy::getAbstract()
{
    std::cout << "erialProxy::getAbstract()" << std::endl;
    if (_material == nullptr) {
        _material = &MaterialBroker::getInstance()->findById(id());
    }
    return _material->getAbstract();
}

#ifndef INITCONTROLLER_H
#define INITCONTROLLER_H

#include "controller.h"
#include <boost/json.hpp>
#include <string>

class InitController:public Controller
{
public:
    static InitController *getInstance();
    boost::json::object getInfo(std::string netizenId);
    boost::json::object getFansAndConsInfo(std::string netizenId);

private:
    InitController();
    static InitController* m_instance;
};

#endif // INITCONTROLLER_H

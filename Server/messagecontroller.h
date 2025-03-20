#ifndef MESSAGECONTROLLER_H
#define MESSAGECONTROLLER_H

#include "controller.h"
#include "mytimer.h"
#include <boost/json.hpp>

class MessageController:public Controller
{
public:
    static MessageController *getInstance();
    boost::json::object getJottingNotification(std::string netizenId);

private:
    void NotificationFlush(std::string netizenId);
    MessageController();
    static MessageController* m_instance;
    MyTimer m_myTimer;
};

#endif // MESSAGECONTROLLER_H

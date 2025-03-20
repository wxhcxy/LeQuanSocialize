#include "initcontroller.h"
#include "netizen.h"
#include "netizenbroker.h"

InitController* InitController::m_instance = nullptr;
InitController *InitController::getInstance()
{
    if(m_instance==nullptr){
        m_instance= new InitController();
    }
    return m_instance;
}

boost::json::object InitController::getInfo(std::string netizenId)
{
    Netizen &netizen=NetizenBroker::getInstance()->findById(netizenId);
    netizen.writeLog();
    netizen.readLog();
    return netizen.getInfo();
}

boost::json::object InitController::getFansAndConsInfo(std::string netizenId)
{
    Netizen &netizen=NetizenBroker::getInstance()->findById(netizenId);
    boost::json::object j;
    j["fans"] = netizen.getFansInfo();
    j["interests"] = netizen.getConcernedInfo();
    return j;
}



InitController::InitController()
{

}

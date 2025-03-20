#ifndef PUBLISHJOTTINGController_H
#define PUBLISHJOTTINGController_H

#include "base64.h"
#include "controller.h"
#include <boost/json.hpp>

class PublishJottingController : public Controller
{
public:
     static PublishJottingController *getInstance();
     void comment(std::string netizenId,std::string comment,std::string jottingId);
     std::string publishJottings(std::string netizenId, boost::json::object j);

 private:
     PublishJottingController();
     static PublishJottingController* m_instance;
};

#endif // PUBLISHJOTTINGController_H

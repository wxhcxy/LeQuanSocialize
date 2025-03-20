#ifndef SCANANDCHECKJOTTINGController_H
#define SCANANDCHECKJOTTINGController_H

#include "controller.h"
#include <boost/json.hpp>

class ScanAndCheckJottingController : public Controller
{
public:
    static ScanAndCheckJottingController *getInstance();
    boost::json::object pushJottings(std::string netizenId);
    boost::json::object pushJottingDetial(std::string netizenId, std::string jottingId);
    boost::json::object pushInfoJottingDetail(std::string netizenId, std::string jottingId);
    boost::json::object scanVideos(std::string netizenId);

private:
    ScanAndCheckJottingController();
    static ScanAndCheckJottingController *m_instance;
};

#endif // SCANANDCHECKJOTTINGController_H

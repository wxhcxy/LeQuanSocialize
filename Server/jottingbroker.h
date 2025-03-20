#ifndef JOTTINGBROKER_H
#define JOTTINGBROKER_H

#include "jotting.h"
#include "jottingproxy.h"
#include "relationalbroker.h"
#include <condition_variable>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

class JottingBroker : public RelationalBroker
{
public:
    static JottingBroker* getInstance();

    //找到某一网民新发布的还未存入数据库中的笔记
    std::vector<std::string> findNewJottings(std::string netizenId);

    std::vector<JottingProxy> pushJottings(std::string netizenId,
                                           std::string lastTime,
                                           std::string thisTime);
    Jotting& findById(std::string id);
    void addJotting(const Jotting& jotting);

    virtual ~JottingBroker();

private:
    JottingBroker();
    Jotting* inCache(std::string id);

    Jotting& retrieveJotting(std::string jottingId);
    std::vector<std::string> findMaterials(std::string jottingid);
    std::vector<std::string> findComments(std::string jottingId);

    static JottingBroker* m_jottingBroker;
    static std::mutex m_mutex;
    static std::unordered_map<std::string, Jotting> m_newClean;
    static std::unordered_map<std::string, Jotting> m_oldClean;
};

#endif // JOTTINGBROKER_H

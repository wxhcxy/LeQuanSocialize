#ifndef NETIZENBROKER_H
#define NETIZENBROKER_H

#include "netizen.h"
#include "relationalbroker.h"
#include <mutex>
#include <set>

class NetizenBroker : public RelationalBroker
{
public:
    static NetizenBroker *getInstance();

    Netizen &findById(std::string id);

    Netizen &retrieveNetizen(std::string netizenId);
    std::vector<std::string> findJottings(std::string netizenId);
    std::vector<std::string> findFans(std::string netizenId);
    std::vector<std::string> findNewFans(std::string netizenId);
    std::vector<std::string> findConcereds(std::string netizenId);
    std::vector<std::string> findComments(std::string netizenId);

    virtual ~NetizenBroker();

    //private:
    NetizenBroker();

    static NetizenBroker *m_netizenBroker;

    //我们不能把静态成员的初始化放置在类的定义中，但是可以在类的外部通过使用范围解析运算符 :: 来重新声明静态变量从而对它进行初始化
    static std::unordered_map<std::string, Netizen> m_newClean;
    static std::unordered_map<std::string, Netizen> m_oldClean;

    //multimap允许重复的键
    static std::multimap<std::string, std::string> m_newFollow;

    static Netizen *inCache(std::string id);

    static void newCleanFlush();
    static void newFollowFlush();

    static std::mutex m_mutex;
};

#endif // NETIZENBROKER_H

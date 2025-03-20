
#include "netizenbroker.h"
#include "commentbroker.h"
#include "jottingbroker.h"
#include <boost/json.hpp>

NetizenBroker* NetizenBroker::m_netizenBroker = NULL;
std::unordered_map<std::string, Netizen> NetizenBroker::m_newClean = {};
std::unordered_map<std::string, Netizen> NetizenBroker::m_oldClean = {};
std::multimap<std::string, std::string> NetizenBroker::m_newFollow = {};

std::mutex NetizenBroker::m_mutex = {};

NetizenBroker* NetizenBroker::getInstance()
{
    if (m_netizenBroker == nullptr) {
        m_netizenBroker = new NetizenBroker();
    }
    return m_netizenBroker;
}

Netizen& NetizenBroker::findById(std::string id)
{
    std::cout << "NetizenBroker::findById(std::string id)" << std::endl;
    Netizen* netizen = inCache(id);
    if (netizen == nullptr) {
        std::cout << "网民id不在缓存中" << std::endl;
        return retrieveNetizen(id);
    }
    return *netizen;
}

Netizen& NetizenBroker::retrieveNetizen(std::string netizenId)
{
    std::cout << "开始在数据库中通过网民id查找网民" << std::endl;
    std::string com = "select * from Netizen where N_id=" + netizenId;
    std::string id, nickName, signal, avater;

    boost::json::object jsonNetizen;
    jsonNetizen = RelationalBroker::retrieveNetizenRelation(com);

    id = jsonNetizen["id"].as_string();
    nickName = jsonNetizen["nickName"].as_string();
    signal = jsonNetizen["signal"].as_string();
    avater = jsonNetizen["avater"].as_string();

    // std::cout << "查找结束!" << std::endl;

    // std::cout << "id: " << id << std::endl;
    // std::cout << "nickName: " << nickName << std::endl;
    // std::cout << "avater: " << avater << std::endl;
    // std::cout << "signal: " << signal << std::endl;

    Netizen netizen(id,
                    nickName,
                    signal,
                    avater,
                    findJottings(id),
                    findFans(id),
                    findConcereds(id),
                    findComments(id));

    std::cout << "开始缓存" << std::endl;
    //将从数据库中拿出的数据放在缓存中(旧的净缓存)
    m_oldClean.insert({netizen.id(), netizen});

    return m_oldClean.at(netizen.id());
}

std::vector<std::string> NetizenBroker::findJottings(std::string netizenId)
{
    //从数据库中将所有netizen发布的jotting的id拿出来
    std::string com = "select J_id from Jotting where N_id=" + netizenId;
    std::vector<std::string> jottingIds = RelationalBroker::findJottingsRelation(com);

    //查看jotting的缓存中是否有该netizen新发布的笔记还未存入数据库
    std::vector<std::string> newCacheJotting = JottingBroker::getInstance()->findNewJottings(
        netizenId);
    jottingIds.insert(jottingIds.end(), newCacheJotting.begin(), newCacheJotting.end());

    return jottingIds;
}

std::vector<std::string> NetizenBroker::findFans(std::string netizenId)
{
    std::string com = "select N_Fan_id from Relation where N_id=" + netizenId;

    std::vector<std::string> fansIds = RelationalBroker::findFansRelation(com);

    std::vector<std::string> newCacheFans = findNewFans(netizenId);
    fansIds.insert(fansIds.end(), newCacheFans.begin(), newCacheFans.end());

    return fansIds;
}

std::vector<std::string> NetizenBroker::findNewFans(std::string netizenId)
{
    std::vector<std::string> fansId;
    for (auto& item : m_newFollow) {
        if (item.first == netizenId) {
            fansId.push_back(item.second);
        }
    }

    return fansId;
}

std::vector<std::string> NetizenBroker::findConcereds(std::string netizenId)
{
    std::string com = "select N_id from Relation where N_Fan_id=" + netizenId;

    std::vector<std::string> concernedIds = RelationalBroker::findConceredsRelation(com);

    std::vector<std::string> newCacheConcerneds = findNewFans(netizenId);
    concernedIds.insert(concernedIds.end(), newCacheConcerneds.begin(), newCacheConcerneds.end());

    return concernedIds;
}

std::vector<std::string> NetizenBroker::findComments(std::string netizenId)
{
    std::string com = "select C_id from Comment where N_id=" + netizenId;
    std::vector<std::string> commentIds = RelationalBroker::findCommentsRelation(com);

    std::vector<std::string> newCacheComments = CommentBroker::getInstance()->findNewComments(
        netizenId);
    commentIds.insert(commentIds.end(), newCacheComments.begin(), newCacheComments.end());

    return commentIds;
}

Netizen* NetizenBroker::inCache(std::string id)
{
    //判断是否在缓存中
    if (m_oldClean.count(id)) {
        return &m_oldClean.at(id);
    }

    if (m_newClean.count(id)) {
        return &m_newClean.at(id);
    }

    return nullptr;
}

NetizenBroker::~NetizenBroker() {}

NetizenBroker::NetizenBroker()
{
    //    m_newFollow.insert({"9","1"});
    //    m_newFollow.insert({"1","9"});

    //    Netizen netizen("9","harward",{},{},{},{});
    //    Netizen netizen1("10","ankewxh",{},{},{},{});
    //    m_newClean.insert({netizen.id(),netizen});
    //    m_newClean.insert({netizen1.id(),netizen1});
}

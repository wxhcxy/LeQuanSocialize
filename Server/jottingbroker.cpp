#include "jottingbroker.h"
#include "commentbroker.h"
#include "materialbroker.h"
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

JottingBroker *JottingBroker::m_jottingBroker = NULL;
std::unordered_map<std::string, Jotting> JottingBroker::m_newClean = {};
std::unordered_map<std::string, Jotting> JottingBroker::m_oldClean = {};
std::mutex JottingBroker::m_mutex = {};

JottingBroker *JottingBroker::getInstance()
{
    if (m_jottingBroker == nullptr) {
        m_jottingBroker = new JottingBroker();
    }
    return m_jottingBroker;
}

std::vector<std::string> JottingBroker::findNewJottings(std::string netizenId)
{
    //网民新发布的笔记应该是存在于newCleanCache中
    std::vector<std::string> res;
    for (auto &item : m_newClean) {
        if (item.second.netizenId() == netizenId) {
            res.push_back(item.first);
        }
    }

    return res;
}

std::vector<JottingProxy> JottingBroker::pushJottings(std::string netizenId,
                                                      std::string lastTime,
                                                      std::string thisTime)
{
    lastTime
        = "2019-12-20 16:00:01"; //"2022-03-24 16:00:01"; //"2021-03-16 20:31:30"; //这行代码是我自己加的测试,因为没有/temp/log.txt文件
    std::cout << "lastTime:" << lastTime << std::endl;
    std::cout << "thisTime:" << thisTime << std::endl;
    //    thisTime="2022-09-25 21:27:59";
    std::string command = "select * from Jotting where J_time BETWEEN '" + lastTime + "' AND '"
                          + thisTime + "'";
    std::vector<std::string> idVec = RelationalBroker::pushJottingsRelation(command);
    std::vector<JottingProxy> ids;
    for (auto &id : idVec) {
        std::cout << "id: " << id << std::endl;
        ids.push_back(JottingProxy(id));
    }
    return ids;
}

Jotting &JottingBroker::findById(std::string id)
{
    Jotting *jotting = inCache(id);

    if (jotting == nullptr) {
        return this->retrieveJotting(id);
    }

    std::cout << "JottingBroker::findById finished" << std::endl;

    return *jotting;
}

void JottingBroker::addJotting(const Jotting &jotting)
{
    //将从数据库中拿出的数据放在缓存中(旧的净缓存)
    m_newClean.insert({jotting.id(), jotting});
}

Jotting *JottingBroker::inCache(std::string id)
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

Jotting &JottingBroker::retrieveJotting(std::string jottingId)
{
    std::string command = "select * from Jotting where J_id=" + jottingId;

    std::string id, content, nid, time;

    boost::json::object jsonJotting = RelationalBroker::retrieveJottingRelation(command);

    id = jsonJotting["id"].as_string();
    content = jsonJotting["content"].as_string();
    nid = jsonJotting["nid"].as_string();
    time = jsonJotting["time"].as_string();

    // std::cout << "查找结束!" << std::endl;
    // std::cout << "id: " << id << std::endl;
    // std::cout << "content: " << content << std::endl;
    // std::cout << "nid: " << nid << std::endl;
    // std::cout << "time: " << time << std::endl;

    //retrieveJotting(id)
    Jotting jotting(id, content, time, nid, findMaterials(id), findComments(id));

    //将从数据库中拿出的数据放在缓存中(旧的净缓存)
    m_oldClean.insert({jotting.id(), jotting});
    std::cout << "JottingBroker::retrieveJotting finished" << std::endl;

    std::cout << "bb: " << m_oldClean.size() << std::endl;
    std::cout << "ccccccccc" << std::endl;

    return m_oldClean.at(jotting.id());

    //    return nullptr;
}

std::vector<std::string> JottingBroker::findMaterials(std::string jottingId)
{
    std::string com = "select M_id from Material where J_id=" + jottingId;
    std::vector<std::string> materialIds = RelationalBroker::findMaterialsRelation(com);

    std::vector<std::string> newMaterials = MaterialBroker::getInstance()->findJottingNewMaterial(
        jottingId);
    materialIds.insert(materialIds.begin(), newMaterials.begin(), newMaterials.end());
    return materialIds;
}

std::vector<std::string> JottingBroker::findComments(std::string jottingId)
{
    std::string com = "select C_id from Comment where J_id=" + jottingId;

    std::vector<std::string> commentIds = RelationalBroker::findCommentsRelation(com);

    int i = 0;
    for (auto &item : commentIds) {
        std::cout << "commentIds " << i << ": " << item << std::endl;
    }

    std::vector<std::string> newComments = CommentBroker::getInstance()->findJottingNewComment(
        jottingId);
    int j = 0;
    for (auto &item : newComments) {
        std::cout << "newComments " << j << ": " << item << std::endl;
    }

    commentIds.insert(commentIds.begin(), newComments.begin(), newComments.end());
    std::cout << "insert" << std::endl;
    return commentIds;
}

JottingBroker::~JottingBroker()
{
    // //程序退出后，将缓存中的新数据存入数据库
    // for (auto &jotting : m_newClean) {
    //     std::string command = "insert into Jotting (J_id,J_content,J_time,N_id) values("
    //                           + jotting.second.id() + "," + jotting.second.note() + ","
    //                           + jotting.second.time() + "," + jotting.second.netizenId() + ")";
    //     RelationalBroker::insert(command);
    // }
}

JottingBroker::JottingBroker()
{
    //    Jotting jotting("8","你好","2025-01-22 10:00:01.00","10",{},{});
    //    m_newClean.insert({jotting.id(),jotting});
    //    Jotting jotting1("10","你好","2025-01-23 10:00:01.00","10",{},{});
    //    m_newClean.insert({jotting1.id(),jotting1});
}

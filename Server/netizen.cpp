#include "netizen.h"
#include "commentbroker.h"
#include "encodephoto.h"
#include "jottingbroker.h"
#include "jottingproxy.h"
#include "materialbroker.h"
#include "messagesequence.h"
#include "snowflake_singleton.h"
#include "snowflakeidworker.h"
#include "test.h"
#include "timeinfo.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

#define TEST_TYPE "PATH0"

Netizen::Netizen(const std::string &tid)
    : NetizenInterface(tid)
{}

Netizen::Netizen(const std::string id,
                 std::string nickName,
                 std::string signal,
                 std::string avatar,
                 std::vector<std::string> jottingId,
                 std::vector<std::string> fansId,
                 std::vector<std::string> concernedsId,
                 std::vector<std::string> commentdId)
    : NetizenInterface(id)
    , m_nickName(nickName)
    , m_signal(signal)
    , m_avatar(avatar)
{
    for (auto &jId : jottingId) {
        _jottings.insert(std::pair<std::string, JottingProxy>(jId, JottingProxy(jId)));
    }
    for (auto &fId : fansId) {
        _fans.insert(std::pair<std::string, NetizenProxy>(fId, NetizenProxy(fId)));
    }
    for (auto &cId : concernedsId) {
        _concerneds.insert(std::pair<std::string, NetizenProxy>(cId, NetizenProxy(cId)));
    }
    for (auto &cId : commentdId) {
        _comments.insert(std::pair<std::string, CommentProxy>(cId, CommentProxy(cId)));
    }
}

boost::json::object Netizen::scanJottings()
{
    //根据时间差推送笔记
    std::vector<JottingProxy> jottingProxys = JottingBroker::getInstance()->pushJottings(id(),
                                                                                         readLog(),
                                                                                         getTime());
    boost::json::object jottings;
    jottings["netizenId"] = id();
    jottings["nickName"] = m_nickName;
    if (strcmp(TEST_TYPE, "PATH")) {
        jottings["avatar"] = encodePhoto(m_avatar);
    } else {
        jottings["avatar"] = m_avatar;
    }
    jottings["avatarId"] = m_avatar;

    // 初始化 "jottings" 为一个数组
    if (!jottings.contains("jottings")) {
        jottings["jottings"] = boost::json::array();
    }
    // 添加到 jotting 对象
    std::cout << "添加jottings: " << jottings << std::endl;

    for (auto &item : jottingProxys) {
        //jottings["jottings"].push_back(item.getAbstract());
        jottings["jottings"].as_array().emplace_back(item.getAbstract());
    }

    return jottings;
}

boost::json::object Netizen::getAbstract()
{
    boost::json::object j;
    j["nickName"] = m_nickName;
    j["signal"] = m_signal;
    if (strcmp(TEST_TYPE, "PATH")) {
        j["avatar"] = encodePhoto(m_avatar);
    } else {
        j["avatar"] = m_avatar;
    }
    j["avatarId"] = m_avatar;
    return j;
}

const std::string Netizen::readLog()
{
    std::cout << "Netizen::readLog() start" << std::endl;
    std::string line, last_line;
    int lineNumber = 0, i = 0;
    std::ifstream logFile;
    //打开log.txt文件
    logFile.open("/temp/log.txt");

    // 检查文件是否成功打开
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file" << std::endl;
        return "";
    }

    //得到所有的行数
    while (std::getline(logFile, line))
        lineNumber++;

    //修改状态标志
    logFile.clear();
    //设置输入位置指示器
    logFile.seekg(0);

    //得到倒数第二行中的内容
    while (std::getline(logFile, last_line) && i < lineNumber - 2)
        i++;

    //关闭文件
    logFile.close();
    //    std::cout<<"line="<<last_line<<std::endl;
    last_line = last_line.substr(4, last_line.length() - 1);
    std::cout << "Netizen::readLog() end" << std::endl;
    return last_line;
}

void Netizen::writeLog()
{
    std::string time_string = getTime();
    std::ofstream logFile;
    //打开log.txt文件，记录本次登陆时间
    logFile.open("/temp/log.txt", std::ios::app | std::ios::out);
    logFile << "in:" << time_string << std::endl;
    logFile.close();
}

boost::json::object Netizen::getInfo()
{
    try {
        std::cout << "getInfo\n";
        boost::json::object netizenInfo;

        // 设置 "name" 字段
        netizenInfo["name"] = m_nickName;
        std::cout << "name\n";

        // 设置 "signal" 字段
        netizenInfo["signal"] = m_signal;
        std::cout << "signal\n";

        // 设置 "avatar" 字段
        if (std::strcmp(TEST_TYPE, "PATH") != 0) {
            netizenInfo["avatar"] = encodePhoto(m_avatar);
        } else {
            netizenInfo["avatar"] = m_avatar;
        }
        std::cout << "avatar\n";

        // 设置 "fanCnt" 字段
        netizenInfo["fanCnt"] = static_cast<int>(_fans.size());
        std::cout << "fanCnt\n";

        // 设置 "interestCnt" 字段
        netizenInfo["interestCnt"] = static_cast<int>(_concerneds.size());
        std::cout << "interestCnt\n";

        // 初始化 "jottings" 为一个数组
        netizenInfo["jottings"] = boost::json::array();
        std::cout << "initializing jottings\n";

        // 遍历 _jottings 并添加每个 jotting 的信息
        for (auto &jp : _jottings) {
            boost::json::object jotting = jp.second.getOnePicAbstract();
            jotting["id"] = jp.first;
            netizenInfo["jottings"].as_array().emplace_back(jotting);
            std::cout << "added a jotting\n";
        }

        // 设置 "fansInfo" 字段
        netizenInfo["fansInfo"] = getFansInfo();
        std::cout << "fansInfo\n";

        // 设置 "concernedInfo" 字段
        netizenInfo["concernedInfo"] = getConcernedInfo();
        std::cout << "concernedInfo\n";

        return netizenInfo;
    } catch (const std::exception &e) {
        std::cout << "getInfo Error: " << e.what() << std::endl;
        // 根据需要，您可以选择抛出异常或返回一个默认的 JSON 对象
        // 这里我们返回一个空的 JSON 对象
        return boost::json::object();
    }
}

// 获取粉丝的抽象信息，返回一个 boost::json::array
boost::json::array Netizen::getFansInfo()
{
    boost::json::array fansInfo;
    for (auto &fp : _fans) {
        fansInfo.emplace_back(fp.second.getAbstract());
    }
    return fansInfo;
}

// 获取关注的抽象信息，返回一个 boost::json::array
boost::json::array Netizen::getConcernedInfo()
{
    boost::json::array concernedsInfo;
    for (auto &cp : _concerneds) {
        concernedsInfo.emplace_back(cp.second.getAbstract());
    }
    return concernedsInfo;
}

boost::json::object Netizen::checkOneJotting(std::string jottingId)
{
    Jotting jotting = JottingBroker::getInstance()->findById(jottingId);
    return jotting.getDetail();
}

void Netizen::updateMessage(std::string jottingId)
{
    _messages.insert(jottingId);
}

std::vector<std::string> Netizen::getJottingNotification()
{
    std::vector<std::string> jottingId;
    for (auto &item : _messages) {
        jottingId.push_back(item);
    }
    return jottingId;
}

bool Netizen::comment(const std::string content, const std::string jottingId)
{
    //获取创建时间
    std::string time = getTime();

    //创建comment的id
    unsigned int comment_id = Singleton<IdWorker>::instance().nextId();

    //创建笔记对象
    Comment *comment = new Comment(std::to_string(comment_id), content, time, id(), jottingId);

    std::cout << "comment id:" << comment->id() << std::endl;
    //将笔记对象存入newCleanCache缓存
    CommentBroker::getInstance()->addComment(*comment);

    //建立netizen和comment的联系
    _comments.insert(
        std::pair<std::string, CommentProxy>(comment->id(), CommentProxy(comment->id())));

    //建立jotting和comment的联系
    Jotting &jotting = JottingBroker::getInstance()->findById(jottingId);
    jotting.comment(comment->id());

    //    delete comment;
    return true;
}

boost::json::object Netizen::checkInfoOneJotting(std::string jottingId)
{
    Jotting jotting = JottingBroker::getInstance()->findById(jottingId);
    return jotting.getInfoDetail();
}

boost::json::object Netizen::scanVideos()
{
    boost::json::object message;

    // 定义视频内容
    std::vector<std::string> contents = {"今日份mv推荐", "我可以自己点燃火炬", "舞蹈训练日常"};

    // 初始化 "videos" 为一个数组
    message["videos"] = boost::json::array();

    for (int i = 1; i < 4; ++i) {
        boost::json::object veidos;

        // 设置 "name" 字段
        veidos["name"] = m_nickName;

        // 设置 "avatarId" 字段
        veidos["avatarId"] = m_avatar;

        // 设置 "avatar" 字段，根据 TEST_TYPE 条件
        if (std::strcmp(TEST_TYPE, "PATH") != 0) {
            veidos["avatar"] = encodePhoto(m_avatar);
        } else {
            veidos["avatar"] = m_avatar;
        }

        // 设置视频路径
        std::string ip = IPADDR;
        std::string hls = "http://" + ip + ":8081/" + std::to_string(i) + "/playlist.m3u8";
        veidos["path"] = hls;

        // 设置 "content" 字段
        veidos["content"] = contents[i - 1];

        // 将 veidos 对象添加到 "videos" 数组中
        message["videos"].as_array().emplace_back(veidos);
    }

    return message;
}

// 发布笔记
bool Netizen::publishJotting(boost::json::object jotting_json)
{
    std::vector<std::string> comments;
    std::vector<Material> materials;
    std::vector<std::string> materialsId;

    // 获取创建时间
    std::string time = getTime();

    // 创建笔记的 ID
    std::string jotting_id = std::to_string(Singleton<IdWorker>::instance().nextId());

    // 获取内容
    std::string content = jotting_json.at("content").as_string().c_str();
    std::cout << "发布笔记的内容为: " << content << std::endl;

    // 处理材料
    for (const auto &material : jotting_json.at("materials").as_array()) {
        std::cout << "所发布笔记的图片大小为: " << material.as_string().c_str() << std::endl;
        std::string materialData = base64_decode(material.as_string().c_str());
        std::string id = std::to_string(Singleton<IdWorker>::instance().nextId());
        std::string path = "/root/ShareBook/Picture/" + id + ".png";

        // 将二进制流转换为文件存储
        std::ofstream fout(path, std::ios::binary);
        fout.write(materialData.c_str(), materialData.length());
        fout.close();

        int type = 1;
        materialsId.push_back(id);
        materials.push_back(Material(id, jotting_id, path, type));
    }

    // 创建笔记对象
    Jotting jotting(jotting_id, content, time, m_id, materialsId, comments);

    // 建立 netizen 和 jotting 的联系
    _jottings.insert(std::pair<std::string, JottingProxy>(jotting.id(), JottingProxy(jotting.id())));

    // 将笔记对象和材料对象存入缓存
    JottingBroker::getInstance()->addJotting(jotting);
    for (const auto &material : materials) {
        MaterialBroker::getInstance()->addMaterial(material);
    }

    // 发送给所有粉丝“发布笔记”的消息
    std::string message_content = "你关注的人有新的笔记";
    unsigned int message_id = Singleton<IdWorker>::instance().nextId();
    MessageSequence::getInstance()->pushNotification(JottingNotification(std::to_string(message_id),
                                                                         m_id,
                                                                         _fans,
                                                                         message_content,
                                                                         time,
                                                                         jotting.id()));

    return true;
}


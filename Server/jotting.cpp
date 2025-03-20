#include "jotting.h"
#include "jottingbroker.h"
#include <functional>
#include <iostream>

#include <utility>

Jotting::Jotting(const std::string &tid, std::string netizenId)
    : JottingInterface(tid)
    , _netizenProxy(netizenId)
    , m_content{}
{}

Jotting::Jotting(const std::string id,
                 std::string content,
                 std::string time,
                 std::string netizenId,
                 std::vector<std::string> materialsId,
                 std::vector<std::string> commentsId)
    : JottingInterface(id)
    , _netizenProxy(netizenId)
    , m_time{time}
    , m_content{content}
{
    for (const auto& item : materialsId) {
        _materials.insert(std::pair<std::string, MaterialProxy>(item, MaterialProxy(item)));
    }
    for (const auto& item : commentsId) {
        _comments.insert(std::pair<std::string, CommentProxy>(item, CommentProxy(item)));
    }
}

boost::json::object Jotting::getAbstract()
{
    boost::json::object j;
    j["id"] = id();
    //    std::cout<<j["id"]<<std::endl;
    j["content"] = m_content;
    j["time"] = m_time;

    // 初始化 "materials" 为一个数组
    if (!j.contains("materials")) {
        j["materials"] = boost::json::array();
    }

    // 示例 1: 添加一个简单的对象
    boost::json::object testMaterial1;
    testMaterial1["picId"] = "test_pic_id_1";
    testMaterial1["picContent"] = "base64_encoded_image_data_1";

    for (auto &m : _materials) {
        // std::cout << m.second.getAbstract() << std::endl;
        j["materials"].as_array().emplace_back(m.second.getAbstract());
        //j["materials"].as_array().emplace_back(testMaterial1);
        std::cout << "完成materials" << std::endl;
        //j["materials"].push_back(m.second.getAbstract());
    }
    j["netizen"] = _netizenProxy.getAbstract();
    return j;
}

const std::string Jotting::netizenId() const
{
    return _netizenProxy.id();
}

boost::json::object Jotting::getOnePicAbstract()
{
    boost::json::object j;
    j["id"] = id();
    j["content"] = m_content;
    j["time"] = m_time;
    if (_materials.size() != 0) {
        j["material_firstPath"] = _materials.begin()->second.getAbstract();
    }
    j["netizen"] = _netizenProxy.getAbstract();
    return j;
}

boost::json::object Jotting::getDetail()
{
    boost::json::object j;
    // 初始化 "comment" 为一个数组
    j["comment"] = boost::json::array();

    for (auto &cm : _comments) {
        j["comment"].as_array().emplace_back(cm.second.getInfo());
    }
    return j;
}

boost::json::object Jotting::getMessageAbstract()
{
    boost::json::object j;
    j["jottingid"] = id();
    j["time"] = m_time;
    j["netizen"] = _netizenProxy.getAbstract();

    return j;
}

void Jotting::comment(const std::string &commentId)
{
    _comments.insert(std::pair<std::string, CommentProxy>(commentId, CommentProxy(commentId)));
}

boost::json::object Jotting::getInfoDetail()
{
    boost::json::object j;
    j["id"] = id();
    j["content"] = m_content;
    j["time"] = m_time;
    j["netizen"] = _netizenProxy.getAbstract();

    // 初始化 "materials" 为一个数组
    if (!j.contains("materials")) {
        j["materials"] = boost::json::array();
    }
    // 初始化 "comment" 为一个数组
    if (!j.contains("comment")) {
        j["comment"] = boost::json::array();
    }

    for (auto &m : _materials) {
        j["materials"].as_array().emplace_back(_materials.begin()->second.getAbstract());
    }
    for (auto &cm : _comments) {
        j["comment"].as_array().emplace_back(cm.second.getInfo());
    }

    return j;
}

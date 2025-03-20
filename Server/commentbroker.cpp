#include "commentbroker.h"
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

CommentBroker *CommentBroker::m_CommentBroker = NULL;
std::unordered_map<std::string, Comment> CommentBroker::m_newClean = {};
std::unordered_map<std::string, Comment> CommentBroker::m_oldClean = {};
std::mutex CommentBroker::m_mutex = {};

CommentBroker *CommentBroker::getInstance()
{
    if (m_CommentBroker == nullptr) {
        m_CommentBroker = new CommentBroker();
    }
    return m_CommentBroker;
}

Comment &CommentBroker::findById(std::string id)
{
    Comment *comment = inCache(id);
    if (comment == nullptr) {
        return retrieveComment(id);
    }
    return *comment;
}

std::vector<std::string> CommentBroker::findJottingNewComment(std::string jottingId)
{
    std::cout << "CommentBroker::findJottingNewComment start" << std::endl;
    std::vector<std::string> commentsId;
    for (auto &item : m_newClean) {
        if (item.second.jottingId() == jottingId) {
            std::cout << "CommentBroker::findJottingNewComment push_back" << std::endl;
            commentsId.push_back(item.first);
        }
    }

    std::cout << "CommentBroker::findJottingNewComment finished" << std::endl;

    return commentsId;
}

Comment &CommentBroker::retrieveComment(std::string commentId)
{
    std::string command = "select * from Comment where C_id=" + commentId;
    boost::json::object jsonComment = RelationalBroker::retrieveCommentRelation(command);
    std::string id, content, nid, jid, time;
    id = jsonComment["id"].as_string();
    content = jsonComment["content"].as_string();
    time = jsonComment["time"].as_string();
    nid = jsonComment["nid"].as_string();
    jid = jsonComment["jid"].as_string();

    //retrieveComment(id)
    Comment comment(id, content, time, nid, jid);

    //将从数据库中拿出的数据放在缓存中(旧的净缓存)
    m_oldClean.insert({comment.id(), comment});

    return m_oldClean.at(comment.id());
}

void CommentBroker::addComment(const Comment &comment)
{
    m_newClean.insert({comment.id(), comment});
}

CommentBroker::~CommentBroker()
{
    //将缓存中的剩余的数据存入数据库
    for (auto &comment : m_newClean) {
        std::string command = "insert into Comment (C_id,C_content,C_time,N_id,J_id) values("
                              + comment.second.id() + ",'" + comment.second.content() + "','"
                              + comment.second.time() + "'," + comment.second.publisherId() + ","
                              + comment.second.jottingId() + ")";
        RelationalBroker::insert(command);
    }
}

CommentBroker::CommentBroker() {}

Comment *CommentBroker::inCache(std::string id)
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

std::vector<std::string> CommentBroker::findNewComments(std::string netizenId)
{
    std::vector<std::string> commentsId;
    for (auto &item : m_newClean) {
        if (item.second.publisherId() == netizenId) {
            commentsId.push_back(item.first);
        }
    }

    return commentsId;
}

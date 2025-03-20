#include "comment.h"

#include <functional>
#include <iostream>

#include <utility>

Comment::Comment(const std::string id,
                 std::string content,
                 std::string time,
                 std::string netizenId,
                 std::string jottingId)
    : CommentInterface(id)
    , _netizenProxy(netizenId)
    , _jottingProxy(jottingId)
    , m_content{content}
    , m_time{time}
{}

boost::json::object Comment::getInfo()
{
    boost::json::object commentInfo;
    commentInfo["content"] = m_content;
    commentInfo["time"] = m_time;
    commentInfo["netizen"] = _netizenProxy.getAbstract();
    return commentInfo;
}

const std::string Comment::content() const
{
    return m_content;
}

const std::string Comment::publisherId() const
{
    return _netizenProxy.id();
}

const std::string Comment::jottingId() const
{
    return _jottingProxy.id();
}

const std::string Comment::time()
{
    return m_time;
}

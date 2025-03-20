#ifndef COMMENTINTERFACE_H
#define COMMENTINTERFACE_H

#include <boost/json.hpp>
#include <string>
#include <unordered_map>

class CommentInterface
{
public:
    explicit CommentInterface(const std::string &tid);
    explicit CommentInterface(const CommentInterface &ci);
    virtual ~CommentInterface() {}

    virtual boost::json::object getInfo() = 0;

    std::string id() const;

private:
    std::string m_timeId;
};

#endif // COMMENTINTERFACE_H

#ifndef JOTTING_H
#define JOTTING_H

#include "commentproxy.h"
#include "jottinginterface.h"
#include "materialproxy.h"
#include "netizenproxy.h"
#include <unordered_map>
#include <vector>

class Jotting : public JottingInterface
{
public:
    explicit Jotting(const std::string &tid, std::string netizenId);
    explicit Jotting(const std::string id,
                     std::string content,
                     std::string time,
                     std::string netizenId,
                     std::vector<std::string> materialsId,
                     std::vector<std::string> commentsId);

    virtual ~Jotting() {}

    virtual boost::json::object getAbstract() override;

    const std::string netizenId() const;
    virtual boost::json::object getOnePicAbstract() override;
    virtual boost::json::object getDetail() override; //主页的笔记详情
    virtual boost::json::object getMessageAbstract() override;
    virtual boost::json::object getInfoDetail() override; //非主页的笔记详情

    void comment(const std::string &commentId);

private:
    NetizenProxy _netizenProxy;

    std::unordered_map<std::string, MaterialProxy> _materials;
    std::unordered_map<std::string, CommentProxy> _comments;

    std::string m_time;
    std::string m_content;
};

#endif // JOTTING_H

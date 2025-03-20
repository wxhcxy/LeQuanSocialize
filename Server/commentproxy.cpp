#include "commentproxy.h"
#include "comment.h"
#include "commentbroker.h"

CommentProxy::CommentProxy(const std::string &tid)
    : CommentInterface{tid}
    , _comment(nullptr)
{}

CommentProxy::~CommentProxy() {}

boost::json::object CommentProxy::getInfo()
{
    if (_comment == nullptr) {
        _comment = &CommentBroker::getInstance()->findById(id());
    }
    return _comment->getInfo();
}

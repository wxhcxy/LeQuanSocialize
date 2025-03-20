#ifndef NETIZENINTERFACE_H
#define NETIZENINTERFACE_H

#include <boost/json.hpp>
#include <iostream>

class NetizenInterface
{
public:
    explicit NetizenInterface(const std::string &tid);
    virtual ~NetizenInterface() {}

    virtual boost::json::object getAbstract() = 0;
    std::string id() const;
    virtual void updateMessage(std::string jottingId) = 0;

private:
    std::string m_id;
};

#endif // NETIZENINTERFACE_H

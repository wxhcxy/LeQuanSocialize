#include "netizeninterface.h"

NetizenInterface::NetizenInterface(const std::string &tid)
    : m_id(tid)
{}

std::string NetizenInterface::id() const
{
    std::cout << "网民id: " << m_id << " "
              << "NetizenInterface::id()" << std::endl;
    return m_id;
}

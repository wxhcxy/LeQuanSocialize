#ifndef RELATIONALBROKER_H
#define RELATIONALBROKER_H
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <mariadb/conncpp.hpp>

#include <boost/json.hpp>

class Netizen;

class RelationalBroker
{
public:
    RelationalBroker();

    void connectMysql();

    boost::json::object retrieveNetizenRelation(std::string command);
    std::vector<std::string> findJottingsRelation(std::string command);
    std::vector<std::string> findFansRelation(std::string command);
    std::vector<std::string> pushJottingsRelation(std::string command);
    std::vector<std::string> findMaterialsRelation(std::string command);
    boost::json::object retrieveJottingRelation(std::string command);
    std::vector<std::string> findCommentsRelation(std::string command);
    std::vector<std::string> findConceredsRelation(std::string command);
    boost::json::object retrieveMaterialRelation(std::string command);
    boost::json::object retrieveCommentRelation(std::string command);

    //sql::ResultSet* query(std::string command);
    void drop(std::string command);
    static void insert(std::string command);

    //清除某些缓存
    virtual ~RelationalBroker() {}
    // private:
    static std::unique_ptr<sql::Connection> m_connection;
};

#endif // RELATIONALBROKER_H

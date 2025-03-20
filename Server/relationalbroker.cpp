#include "relationalbroker.h"

#include <iostream>
#include <string>

std::unique_ptr<sql::Connection> RelationalBroker::m_connection = NULL;

RelationalBroker::RelationalBroker()
{
    // std::cout << "连接数据库" << std::endl;
    // // Instantiate Driver
    // sql::Driver* driver = sql::mariadb::get_driver_instance();

    // // Configure Connection
    // sql::SQLString url("jdbc:mariadb://localhost:3306/ShareBook");
    // sql::Properties properties({{"user", "root"}, {"password", "123456"}});

    // // Establish Connection
    // std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
    // m_connection = std::move(conn);

    // // 验证连接是否有效
    // if (m_connection->isValid(1000)) { // 1秒超时
    //     std::cout << "连接验证通过！" << std::endl;
    // } else {
    //     std::cerr << "连接验证失败！" << std::endl;
    // }
    connectMysql();
}

void RelationalBroker::connectMysql()
{
    std::cout << "连接数据库" << std::endl;
    // Instantiate Driver
    sql::Driver* driver = sql::mariadb::get_driver_instance();

    // Configure Connection
    sql::SQLString url("jdbc:mariadb://localhost:3306/ShareBook");
    sql::Properties properties({{"user", "root"}, {"password", "123456"}});

    // Establish Connection
    std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
    m_connection = std::move(conn);

    // 验证连接是否有效
    if (m_connection->isValid(1000)) { // 1秒超时
        std::cout << "连接验证通过！" << std::endl;
    } else {
        std::cerr << "连接验证失败！" << std::endl;
    }
}

boost::json::object RelationalBroker::retrieveNetizenRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        // Create a new Statement
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        // Execute query
        sql::ResultSet* res = stmnt->executeQuery(command);
        std::cout << "查到数据库数据 " << std::endl;
        boost::json::object jsonNetizenInfo;
        while (res->next()) {
            jsonNetizenInfo["id"] = std::to_string(res->getUInt(1));
            jsonNetizenInfo["nickName"] = res->getString(2);
            jsonNetizenInfo["avater"] = res->getString(3);
            jsonNetizenInfo["signal"] = res->getString(4);

            // std::cout << "id: " << std::to_string(res->getUInt(1)) << std::endl;
            // std::cout << "content: " << res->getString(2) << std::endl;
            // std::cout << "nid: " << res->getString(3) << std::endl;
            // std::cout << "time: " << res->getString(4) << std::endl;
        }
        return jsonNetizenInfo;

    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
    return {};
}

std::vector<std::string> RelationalBroker::findJottingsRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);
        std::vector<std::string> jottingIds;
        while (res->next()) {
            jottingIds.push_back(std::to_string(res->getUInt(1)));
        }
        return jottingIds;

    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

std::vector<std::string> RelationalBroker::findFansRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);
        std::vector<std::string> fansIds;
        while (res->next()) {
            fansIds.push_back(std::to_string(res->getUInt(1)));
        }
        return fansIds;

    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

std::vector<std::string> RelationalBroker::pushJottingsRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);
        std::string id;
        std::vector<std::string> idVec;
        while (res->next()) {
            id = std::to_string(res->getUInt(1));
            idVec.push_back(id);
        }
        return idVec;

    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

boost::json::object RelationalBroker::retrieveJottingRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);
        boost::json::object jsonJottingInfo;
        while (res->next()) {
            jsonJottingInfo["id"] = std::to_string(res->getUInt(1));
            jsonJottingInfo["content"] = res->getString(2);
            jsonJottingInfo["time"] = res->getString(3);
            jsonJottingInfo["nid"] = std::to_string(res->getUInt(4));
            // std::cout << "id: " << std::to_string(res->getUInt(1)) << std::endl;
            // std::cout << "content: " << res->getString(2) << std::endl;
            // std::cout << "time: " << res->getString(3) << std::endl;
            // std::cout << "nid: " << std::to_string(res->getUInt(4));
        }
        return jsonJottingInfo;

    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
    return {};
}

std::vector<std::string> RelationalBroker::findMaterialsRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);

        std::vector<std::string> materialIds;
        while (res->next()) {
            materialIds.push_back(std::to_string(res->getUInt(1)));
        }
        return materialIds;
    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

std::vector<std::string> RelationalBroker::findCommentsRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::cout << "111111111" << std::endl;
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);
        std::vector<std::string> commentIds;
        while (res->next()) {
            commentIds.push_back(std::to_string(res->getUInt(1)));
            std::cout << "comment: " << std::to_string(res->getUInt(1)) << std::endl;
        }
        //std::cout << "commentIds: " << commentIds.size() << std::endl;
        return commentIds;
    } catch (sql::SQLException& e) {
        std::cout << "22222" << std::endl;
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

std::vector<std::string> RelationalBroker::findConceredsRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);
        std::vector<std::string> concernedIds;
        while (res->next()) {
            concernedIds.push_back(std::to_string(res->getUInt(1)));
        }
        return concernedIds;
    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
}

boost::json::object RelationalBroker::retrieveMaterialRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);
        boost::json::object jsonMaterialInfo;
        while (res->next()) {
            jsonMaterialInfo["id"] = std::to_string(res->getUInt(1));
            jsonMaterialInfo["path"] = res->getString(2);
            jsonMaterialInfo["type"] = res->getUInt(3);
            jsonMaterialInfo["jId"] = std::to_string(res->getUInt(4));
        }
        return jsonMaterialInfo;

    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
    return {};
}

boost::json::object RelationalBroker::retrieveCommentRelation(std::string command)
{
    std::cout << "query command: " << command << std::endl;
    try {
        std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
        sql::ResultSet* res = stmnt->executeQuery(command);
        boost::json::object jsonCommentInfo;
        while (res->next()) {
            jsonCommentInfo["id"] = std::to_string(res->getUInt(1));
            jsonCommentInfo["content"] = res->getString(2);
            jsonCommentInfo["time"] = res->getString(3);
            jsonCommentInfo["nid"] = std::to_string(res->getUInt(4));
            jsonCommentInfo["jid"] = res->getString(5);
        }
        return jsonCommentInfo;

    } catch (sql::SQLException& e) {
        std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
    return {};
}

// sql::ResultSet* RelationalBroker::query(std::string command)
// {
//     // std::cout << "query command: " << command << std::endl;
//     // try {
//     //     // Create a new Statement
//     //     std::unique_ptr<sql::Statement> stmnt(m_connection->createStatement());
//     //     // Execute query
//     //     sql::ResultSet* res = stmnt->executeQuery(command);
//     //     std::cout << "查到数据库数据 " << std::endl;
//     //     // while (res->next()) {
//     //     //     std::cout << "id: " << std::to_string(res->getUInt(1)) << std::endl;
//     //     //     std::cout << "nickName: " << res->getString(2) << std::endl;
//     //     //     std::cout << "avater: " << res->getString(3) << std::endl;
//     //     //     std::cout << "signal: " << res->getString(4) << std::endl;
//     //     // }
//     //     return res;

//     // } catch (sql::SQLException& e) {
//     //     std::cerr << "Error selecting tasks: " << e.what() << std::endl;
//     // }
//     // std::cout << "没查到数据，return nullptr RelationalBroker::query(std::string command)"
//     //           << std::endl;
//     // return nullptr;
// }

void RelationalBroker::insert(std::string command)
{
    try {
        std::unique_ptr<sql::PreparedStatement> stmnt(m_connection->prepareStatement(command));
        stmnt->executeQuery();
    } catch (sql::SQLException& e) {
        std::cerr << "Error inserting new task: " << e.what() << std::endl;
    }
}

void RelationalBroker::drop(std::string command)
{
    try {
        std::unique_ptr<sql::PreparedStatement> stmnt(m_connection->prepareStatement(command));
        stmnt->executeQuery();
    } catch (sql::SQLException& e) {
        std::cerr << "Error delete task: " << e.what() << std::endl;
    }
}

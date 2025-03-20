#include "pushpagecontrol.h"

#include "client.h"
#include "clientInfo.h"
#include "base64.h"
#include "imageprovider.h"

#include <boost/json.hpp>

extern ImageProvider *imageProvider;

PushPageControl::PushPageControl(QObject *parent):
    QObject(parent)
{

}

void PushPageControl::pushRecommendJottings()
{
    // 发送浏览笔记请求，请求服务端推送笔记
    boost::json::object message;
    message["id"] = ID;
    message["request"] = "ScanJottings";

    // 序列化 JSON 对象为字符串
    std::string sendData = boost::json::serialize(message);
    std::cout << "客户端发送消息：" << sendData << std::endl;
    Client::getInstance()->send(sendData.c_str(), sendData.length());
    std::cout << "客户端获取用户推送笔记的信息......" << std::endl;

    // 接收数据
    char *receiveData = new char[100000000];
    Client::getInstance()->receive(receiveData);
    std::cout << "完整收到receive" << std::endl;

    // 将接收到的数据转换为 std::string
    std::string json_str(receiveData);
    delete[] receiveData;
    //std::cout << json_str << std::endl;

    // 解析 JSON 字符串为 boost::json::value
    boost::json::error_code ec;
    boost::json::value parsed = boost::json::parse(json_str, ec);
    //std::cout << parsed << std::endl;
    if (ec) {
        qDebug() << "JSON parse error:" << QString::fromStdString(ec.message());
        return;
    }
    std::cout << "转换完成json" << std::endl;

    // 获取到的网民基本信息 以 json形式
    // 转换为 boost::json::object
    boost::json::object scanInfo;
    try {
        scanInfo = parsed.as_object();
        //std::cout << scanInfo << std::endl;
    } catch (const std::exception &e) {
        qDebug() << "JSON conversion error:" << QString::fromStdString(e.what());
        return;
    }

    // //测试输出
    // qDebug() << "m_nickName:" << QString::fromStdString(scanInfo["nickName"].as_string().c_str());
    // std::string avatarId = scanInfo["avatarId"].as_string().c_str();
    // std::string path = "image://photos/" + avatarId;
    // std::cout << "avatarId: " << avatarId << std::endl;
    // std::cout << "path: " << path << std::endl;

    // 昵称
    //if (scanInfo.contains("nickName") && scanInfo["nickName"].is_string()) {
    m_nickName = QString::fromStdString(scanInfo["nickName"].as_string().c_str());
    // } else {
    //     qDebug() << "Missing or invalid 'nickName' field";
    // }
    qDebug() << "m_nickName:" << QString::fromStdString(scanInfo["nickName"].as_string().c_str());

    // 获取头像信息
    // if (scanInfo.contains("avatarId") && scanInfo["avatarId"].is_string()
    //     && scanInfo.contains("avatar") && scanInfo["avatar"].is_string()) {
    std::string avatarId = scanInfo["avatarId"].as_string().c_str();
    std::string path = "image://photos/" + avatarId;
    std::cout << "avatarId: " << avatarId << std::endl;
    std::cout << "path: " << path << std::endl;
    //std::cout << "avatar: " << scanInfo["avatar"].as_string().c_str() << std::endl;

    std::string avatarData = base64_decode(scanInfo["avatar"].as_string().c_str());
    QPixmap newpixmap;
    newpixmap.loadFromData(reinterpret_cast<const unsigned char *>(avatarData.c_str()),
                           static_cast<int>(avatarData.length()));
    imageProvider->setAvatar(newpixmap);
    m_avatar = QString::fromStdString(path);
    // } else {
    //     qDebug() << "Missing or invalid 'avatarId' or 'avatar' field";
    // }

    std::cout << "头像信息获取完毕" << std::endl;

    boost::json::object receiveJottings;
    QHash<QString, QPixmap> avatars;
    QHash<QString, QPixmap> allPics;

    //std::cout << scanInfo["jottings"].as_array() << std::endl;

    for (const auto &info : scanInfo["jottings"].as_array()) {
        boost::json::object infoObj;

        infoObj = info.as_object();

        boost::json::object jotting;
        jotting["id"] = infoObj["id"].as_string().c_str();

        boost::json::object netizen = infoObj["netizen"].as_object();
        jotting["nickName"] = netizen["nickName"].as_string().c_str();
        jotting["content"] = infoObj["content"].as_string().c_str();

        std::string avatarId = netizen["avatarId"].as_string().c_str();
        std::string avatarPath = "image://photos/" + avatarId;
        std::string avatarData = base64_decode(netizen["avatar"].as_string().c_str());
        std::cout << "avatarId: " << avatarId << std::endl;
        std::cout << "avatarPath: " << avatarPath << std::endl;
        //std::cout << "avatarData: " << avatarData << std::endl;

        QPixmap pixmap;
        pixmap.loadFromData(reinterpret_cast<const unsigned char *>(avatarData.c_str()),
                            static_cast<int>(avatarData.length()));
        avatars.insert(QString::fromStdString(avatarId), pixmap);
        jotting["avatarPath"] = avatarPath.c_str();

        // 笔记时间
        jotting["time"] = infoObj["time"].as_string().c_str();

        std::cout << "笔记时间完毕" << std::endl;

        //sleep(2);

        // 笔记素材信息
        boost::json::object picPath;
        boost::json::array materials = infoObj["materials"].as_array();
        //std::cout << materials << std::endl;

        for (const auto &material : materials) {
            boost::json::object materialObj = material.as_object();
            std::string picId = materialObj["picId"].as_string().c_str();
            std::string picNewPath = "image://photos/" + picId;
            std::cout << "picId: " << picId << std::endl;
            std::cout << "picNewPath: " << picNewPath << std::endl;

            // 图片解码
            QPixmap newpixmap;
            std::string picData = base64_decode(materialObj["picContent"].as_string().c_str());
            newpixmap.loadFromData(reinterpret_cast<const unsigned char *>(picData.c_str()),
                                   static_cast<int>(picData.length()));

            picPath["path"] = picNewPath;

            // 初始化 "picPath" 为一个数组
            if (!jotting.contains("picPath")) {
                jotting["picPath"] = boost::json::array();
            }
            // 添加到 jotting 对象
            std::cout << "1: " << jotting << std::endl;
            jotting["picPath"].as_array().emplace_back(picPath);
            std::cout << "2: " << jotting << std::endl;
            // 添加到哈希表
            allPics.insert(QString::fromStdString(picId), newpixmap);
        }
        // 添加到 receiveJottings 对象
        // 初始化 "jottings" 为一个数组
        if (!receiveJottings.contains("jottings")) {
            receiveJottings["jottings"] = boost::json::array();
        }
        receiveJottings["jottings"].as_array().emplace_back(jotting);
    }
    // 设置图片哈希表
    imageProvider->setpushUIAvatars(avatars);
    imageProvider->setpushUIPics(allPics);

    // 输出接收到的笔记信息
    std::cout << boost::json::serialize(receiveJottings) << std::endl;

    // 序列化 receiveJottings 为字符串
    m_recommendJottings = QString::fromStdString(boost::json::serialize(receiveJottings));
    qDebug() << m_recommendJottings;

    // 输出笔记个数
    std::cout << "笔记个数：" << scanInfo["jottings"].as_array().size() << std::endl;
}

void PushPageControl::pushConcernedJottings()
{
    // 创建第一个笔记对象 (jot_json1)
    boost::json::object jot_json1;
    jot_json1["avatar"] = "qrc:/images/images/avatar.png";
    jot_json1["nickName"] = "Baby";
    jot_json1["content"] = "We could leave the Christmas";

    // 创建第一个 picPath 对象
    boost::json::object picPath1;
    picPath1["path"] = "qrc:/images/images/avatar.png";

    // 将 picPath1 添加到 picPaths 数组中
    boost::json::array picPaths1;
    picPaths1.emplace_back(picPath1);
    picPaths1.emplace_back(picPath1);
    picPaths1.emplace_back(picPath1);

    // 将 picPaths1 赋值给 jot_json1 的 "picPath" 字段
    jot_json1["picPath"] = picPaths1;

    // 创建第二个笔记对象 (jot_json2)
    boost::json::object jot_json2;
    jot_json2["avatar"] = "qrc:/images/images/avatar.png";
    jot_json2["nickName"] = "Angel";
    jot_json2["content"] = "This is our place, we make the rules";

    // 创建第二个 picPath 对象
    boost::json::object picPath2;
    picPath2["path"] = "qrc:/images/images/avatar.png";

    // 将 picPath2 添加到 picPaths 数组中
    boost::json::array picPaths2;
    picPaths2.emplace_back(picPath2);
    picPaths2.emplace_back(picPath2);
    picPaths2.emplace_back(picPath2);

    // 将 picPaths2 赋值给 jot_json2 的 "picPath" 字段
    jot_json2["picPath"] = picPaths2;

    // 创建 jottings 数组并添加 jot_json1 和 jot_json2
    boost::json::array jottings_array;
    jottings_array.emplace_back(jot_json1);
    jottings_array.emplace_back(jot_json2);

    // 创建最终的 jottings 对象
    boost::json::object jottings;
    jottings["jottings"] = jottings_array;

    // 将 boost::json::object 序列化为字符串
    std::string json_str = boost::json::serialize(jottings);

    // 将字符串转换为 QString 并赋值给 m_concernedJottings
    m_concernedJottings = QString::fromStdString(json_str);

    // 输出最终的 JSON 字符串（可选，用于调试）
    qDebug() << "Concerned Jottings JSON:" << m_concernedJottings;
}

void PushPageControl::pushLocalJottings()
{
    // 创建第一个笔记对象 (jot_json1)
    boost::json::object jot_json1;
    jot_json1["avatar"] = "qrc:/images/images/avatar.png";
    jot_json1["nickName"] = "Baby";
    jot_json1["content"] = "We could leave the Christmas";

    // 创建第一个 picPath 对象
    boost::json::object picPath1;
    picPath1["path"] = "qrc:/images/images/avatar.png";

    // 创建 picPaths1 数组并添加 picPath1 三次
    boost::json::array picPaths1;
    picPaths1.emplace_back(picPath1);
    picPaths1.emplace_back(picPath1);
    picPaths1.emplace_back(picPath1);

    // 将 picPaths1 赋值给 jot_json1 的 "picPath" 字段
    jot_json1["picPath"] = picPaths1;

    // 创建第二个笔记对象 (jot_json2)
    boost::json::object jot_json2;
    jot_json2["avatar"] = "qrc:/images/images/avatar.png";
    jot_json2["nickName"] = "Angel";
    jot_json2["content"] = "This is our place, we make the rules";

    // 创建第二个 picPath 对象
    boost::json::object picPath2;
    picPath2["path"] = "qrc:/images/images/avatar.png";

    // 创建 picPaths2 数组并添加 picPath2 三次
    boost::json::array picPaths2;
    picPaths2.emplace_back(picPath2);
    picPaths2.emplace_back(picPath2);
    picPaths2.emplace_back(picPath2);

    // 将 picPaths2 赋值给 jot_json2 的 "picPath" 字段
    jot_json2["picPath"] = picPaths2;

    // 创建 jottings 数组并添加 jot_json1 和 jot_json2
    boost::json::array jottings_array;
    jottings_array.emplace_back(jot_json1);
    jottings_array.emplace_back(jot_json2);

    // 创建最终的 jottings 对象
    boost::json::object jottings;
    jottings["jottings"] = jottings_array;

    // 将 boost::json::object 序列化为字符串
    std::string json_str = boost::json::serialize(jottings);

    // 将字符串转换为 QString 并赋值给 m_localJottings
    m_localJottings = QString::fromStdString(json_str);

    // 输出最终的 JSON 字符串（可选，用于调试）
    qDebug() << "Local Jottings JSON:" << m_localJottings;
}

void PushPageControl::commentJotting(QString text, QString jottingId)
{
    qDebug() << "进入函数： PushPageControl::commentJotting";

    // 构建 JSON 对象
    boost::json::object message;
    message["id"] = ID;
    message["jottingId"] = jottingId.toStdString();
    message["text"] = text.toStdString();
    message["request"] = "Comment";

    // 序列化 JSON 对象为字符串
    std::string sendData = boost::json::serialize(message);
    qDebug() << "序列化后的发送数据:" << QString::fromStdString(sendData);

    // 重新连接并发送数据
    Client::getInstance()->reconnect();
    Client::getInstance()->send(sendData.c_str(), sendData.length());
    std::cout << "Client"
              << "客户端发送评论到服务器成功！......" << std::endl;
}

void PushPageControl::receiveOneJotting(QString jotting_id)
{
    qDebug() << "进来函数： PushPageControl::receiveOneJotting";

    // 解析推荐笔记的 JSON 字符串
    boost::json::error_code ec;
    boost::json::value recommendJottingsValue = boost::json::parse(m_recommendJottings.toStdString(),
                                                                   ec);
    if (ec) {
        qDebug() << "JSON parse error in recommendJottings:"
                 << QString::fromStdString(ec.message());
        return;
    }
    boost::json::object recommendJottings = recommendJottingsValue.as_object();

    // 查找匹配的笔记
    std::string id = jotting_id.toStdString();
    boost::json::object jotting;
    bool found = false;

    for (auto &jot : recommendJottings["jottings"].as_array()) {
        if (jot.as_object().contains("id") && jot.as_object()["id"].as_string() == id) {
            jotting["id"] = id;
            jotting["avatar"] = jot.as_object()["avatarPath"].as_string().c_str();
            jotting["netizenName"] = jot.as_object()["nickName"].as_string().c_str();
            jotting["picPath"] = jot.as_object()["picPath"];
            jotting["time"] = jot.as_object()["time"].as_string().c_str();
            jotting["content"] = jot.as_object()["content"].as_string().c_str();
            found = true;
            break;
        }
    }

    if (!found) {
        qDebug() << "未找到匹配的笔记 ID:" << jotting_id;
        return;
    }

    // 发送请求以获取笔记的详细信息
    boost::json::object message;
    message["id"] = ID;
    message["jottingId"] = id;
    message["request"] = "GetJottingDetail";

    std::string sendData = boost::json::serialize(message);
    Client::getInstance()->reconnect();
    Client::getInstance()->send(sendData.c_str(), sendData.length());
    std::cout << "Client"
              << "客户端获取用户推送笔记的详细信息......" << std::endl;

    // 接收数据
    char *receiveData = new char[10000000];
    Client::getInstance()->receive(receiveData);

    // 解析接收到的评论信息
    boost::json::error_code ec_comments;
    boost::json::value commentsInfoValue = boost::json::parse(receiveData, ec_comments);
    delete[] receiveData;
    if (ec_comments) {
        qDebug() << "JSON parse error in commentsInfo:"
                 << QString::fromStdString(ec_comments.message());
        return;
    }
    boost::json::object commentsInfo = commentsInfoValue.as_object();

    // 处理评论信息
    QHash<QString, QPixmap> avatars;
    boost::json::array commentsArray = commentsInfo["comment"].as_array();
    for (auto &com : commentsArray) {
        boost::json::object comment;
        comment["netizenName"]
            = com.as_object()["netizen"].as_object()["nickName"].as_string().c_str();
        comment["content"] = com.as_object()["content"].as_string().c_str();
        comment["time"] = com.as_object()["time"].as_string().c_str();

        // 处理笔记作者头像信息
        std::string avatarId = com.as_object()["netizen"].as_object()["avatarId"].as_string().c_str();
        std::string avatarPath = "image://photos/" + avatarId;
        std::string avatarData = base64_decode(
            com.as_object()["netizen"].as_object()["avatar"].as_string().c_str());
        QPixmap pixmap;
        pixmap.loadFromData(reinterpret_cast<const unsigned char *>(avatarData.c_str()),
                            static_cast<int>(avatarData.length()));
        avatars.insert(QString::fromStdString(avatarId), pixmap);
        comment["pic"] = avatarPath.c_str();

        // 将评论添加到 jotting 对象中
        // 添加到 receiveJottings 对象
        // 初始化 "jottings" 为一个数组
        if (!jotting.contains("comment")) {
            jotting["comment"] = boost::json::array();
        }
        jotting["comment"].as_array().emplace_back(comment);
    }

    // 设置头像信息到图像提供者
    imageProvider->setDetailUIAvatars(avatars);

    // 将 jotting 对象序列化为字符串并赋值给 m_jotting
    m_jotting = QString::fromStdString(boost::json::serialize(jotting));

    std::cout << "接收到的笔记详情:" << std::endl << m_jotting.toStdString() << std::endl;
}

void PushPageControl::setNickName(const QString &nickName)
{
    m_nickName = nickName;
}

QString PushPageControl::getNickName() const
{
    return m_nickName;
}

void PushPageControl::setAvatar(const QString &avatar)
{
    m_avatar = avatar;
    emit avatarChanged(m_avatar);
}

QString PushPageControl::getAvatar() const
{
    return m_avatar;
}
void PushPageControl::setRecommendJottings(const QString &recommendJottings)
{
    m_recommendJottings = recommendJottings;
}

QString PushPageControl::getRecommendJottings() const
{
    return m_recommendJottings;
}

void PushPageControl::setConcernedJottings(const QString &concernedJottings)
{
    m_concernedJottings = concernedJottings;
}

QString PushPageControl::getConcernedJottings() const
{
    return m_concernedJottings;
}

void PushPageControl::setLocalJottings(const QString &localJottings)
{
    m_localJottings = localJottings;
}

QString PushPageControl::getLocalJottings() const
{
    return m_localJottings;
}

void PushPageControl::setJotting(const QString &jotting)
{
    m_jotting = jotting;
}

QString PushPageControl::getJotting() const
{
    return m_jotting;
}

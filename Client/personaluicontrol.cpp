#include "personaluicontrol.h"

#include "base64.h"
#include "client.h"
#include "clientInfo.h"
#include "imageprovider.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <boost/json.hpp>

extern ImageProvider *imageProvider;
//ImageProvider *imageProvider;

PersonalUIControl::PersonalUIControl(QObject *parent):
    QObject(parent),m_nickName(""),m_sign("")
{

}

void PersonalUIControl::setNickName(const QString &nickName)
{
    m_nickName=nickName;
}

QString PersonalUIControl::getNickName() const
{
    return m_nickName;
}

void PersonalUIControl::setSign(const QString &sign)
{
    m_sign=sign;
    emit signChanged(m_sign);
}

QString PersonalUIControl::getSign() const
{
    return m_sign;
}

void PersonalUIControl::setAvatar(const QString &avatar)
{
    m_avatar=avatar;
    emit avatarChanged(m_avatar);
}

QString PersonalUIControl::getAvatar() const
{
    return m_avatar;
}

void PersonalUIControl::setJotting(const QString &jotting)
{
     m_jotting=jotting;
}

QString PersonalUIControl::getJotting() const
{
    return m_jotting;
}


void PersonalUIControl::setJottings(const QString &jottings)
{
    m_jottings=jottings;
    emit jottingsChanged(m_jottings);
}

QString PersonalUIControl::getJottings() const
{
    return m_jottings;
}


void PersonalUIControl::setInterest(const QString &interest)
{
    m_interest=interest;
    emit interestChanged(m_interest);
}

QString PersonalUIControl::getInterest() const
{
    return m_interest;
}

void PersonalUIControl::setFans(const QString &fans)
{
    m_fans=fans;
    emit fansChanged(m_fans);
}

QString PersonalUIControl::getFans() const
{
    return m_fans;
}

void PersonalUIControl::setJottingCount(const QString &jottingCount)
{
    m_jotting_count=jottingCount;
    emit jottingCountChanged(m_jotting_count);
}

QString PersonalUIControl::getJottingCount() const
{
    return m_jotting_count;
}

void PersonalUIControl::setInterestCount(const QString &interestCount)
{
    m_interest_count=interestCount;
    emit interestCountChanged(m_interest_count);
}

QString PersonalUIControl::getInterestCount() const
{
    return m_interest_count;
}

void PersonalUIControl::setFanCount(const QString &fanCount)
{
    m_fan_count=fanCount;
    emit fanCountChanged(m_fan_count);
}

QString PersonalUIControl::getFanCount() const
{
    return m_fan_count;
}

QJsonValue stringToJson(const QString &str)
{
    QString context = QString("{%1}").arg(str);
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(context.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        return QJsonValue();
    }

    return doc.object();
}

QJsonObject transition(boost::json::object js)
{
    // 将 boost::json 对象序列化为字符串
    std::string info = boost::json::serialize(js);

    // 将字符串转换为 QByteArray
    QByteArray byte_array = QByteArray::fromStdString(info);

    // 解析 QByteArray 为 QJsonDocument
    QJsonParseError json_error;
    QJsonDocument parse_document = QJsonDocument::fromJson(byte_array, &json_error);

    // 检查解析是否成功
    if (json_error.error == QJsonParseError::NoError) {
        if (parse_document.isObject()) {
            return parse_document.object();
        } else {
            qDebug() << "Parsed JSON is not an object";
        }
    } else {
        qDebug() << "JSON parse error:" << json_error.errorString();
    }

    // 返回一个空的 QJsonObject 如果解析失败
    return QJsonObject();
}

void PersonalUIControl::initData()
{
    // 创建 JSON 对象并赋值
    boost::json::object message;
    message["id"] = ID;
    message["request"] = "InitPersonalInfo";

    // 序列化 JSON 对象
    std::string sendData = boost::json::serialize(message);

    std::cout << "客户端发送数据: " << sendData << std::endl;
    // 发送数据
    Client::getInstance()->reconnect();
    Client::getInstance()->send(sendData.c_str(), sendData.length());

    std::cout << "客户端初始化用户信息......" << std::endl;

    // 接收数据
    char *receiveData = new char[9999999];
    Client::getInstance()->receive(receiveData);

    // 将接收到的数据转换为 std::string
    std::string json_str(receiveData);
    delete[] receiveData;

    // 解析 JSON 字符串为 boost::json::value
    boost::json::error_code ec;
    boost::json::value parsed = boost::json::parse(json_str, ec);

    if (ec) {
        qDebug() << "JSON parse error:" << QString::fromStdString(ec.message());
        return;
    }

    // 转换为 boost::json::object
    boost::json::object personInfo = parsed.as_object();

    // 转换为 QJsonObject
    QJsonObject rootObj = transition(personInfo);

    // 昵称
    m_nickName = rootObj["name"].toString();

    // 个签
    m_sign = rootObj["signal"].toString();

    // 粉丝数量
    m_fan_count = QString::number(rootObj["fanCnt"].toInt());

    m_interest_count = QString::number(rootObj["interestCnt"].toInt());

    // 获取头像信息
    std::string avatarData = base64_decode(personInfo["avatar"].as_string().c_str());
    QPixmap newpixmap;
    newpixmap.loadFromData(reinterpret_cast<const unsigned char *>(avatarData.c_str()),
                           static_cast<int>(avatarData.length()));

    imageProvider->setAvatar(newpixmap);

    // 获取笔记信息
    QJsonArray jottingsArray = rootObj["jottings"].toArray();
    QJsonArray array;
    QHash<QString, QPixmap> firstPicHash;

    for (int i = 0; i < jottingsArray.size(); i++) {
        QJsonObject jot_Obj = jottingsArray[i].toObject();

        // 提取图片信息
        boost::json::object jottingObj = personInfo["jottings"].as_array()[i].as_object();
        boost::json::object materialFirstPathObj = jottingObj["material_firstPath"].as_object();

        std::string imgId = materialFirstPathObj["picId"].as_string().c_str();
        std::string data = base64_decode(materialFirstPathObj["picContent"].as_string().c_str());
        QPixmap pixmap;
        pixmap.loadFromData(reinterpret_cast<const unsigned char *>(data.c_str()),
                            static_cast<int>(data.length()));

        firstPicHash.insert(QString::fromStdString(imgId), pixmap);

        // 构建图片路径
        std::string picPath = "image://photos/" + imgId;

        // 插入数据到 QJsonObject
        jot_Obj.insert("jottingId", QString::fromStdString(jottingObj["id"].as_string().c_str()));
        jot_Obj.insert("picPath", QString::fromStdString(picPath));
        jot_Obj.insert("file", "JotDetailPage.qml");

        // 添加到数组中
        array.append(QJsonValue(jot_Obj));
    }

    // 设置图片哈希表
    imageProvider->setHashPixmaps(firstPicHash);

    // 设置笔记数量
    m_jotting_count = QString::number(array.count());

    // 序列化 QJsonArray 为字符串
    QJsonDocument jot_doc(array);
    m_jottings = QString(jot_doc.toJson(QJsonDocument::Indented));

    // 关注者信息
    qDebug() << "关注者信息。。。。。。。。。。。。。";

    QJsonArray arrayIn;
    QHash<QString, QPixmap> interestAvatorVec;

    // 检查是否有关注者
    if (personInfo["concernedInfo"].is_null()) {
        qDebug() << "没有关注者。。。。。。。。。。。。。";
        QJsonObject interest_json1;
        interest_json1.insert("nickName", "NULL");
        interest_json1.insert("signal", "NULL");
        interest_json1.insert("avatarPath", "NULL");
        interest_json1.insert("file", "qrc:/qml/qml/PersonalPage.qml");
        arrayIn.append(QJsonValue(interest_json1));
    } else {
        QJsonArray interestsArray = rootObj["concernedInfo"].toArray();
        qDebug() << interestsArray.isEmpty();

        for (int i = 0; i < interestsArray.size(); i++) {
            QJsonObject interest_Obj = interestsArray[i].toObject();

            // 提取头像信息
            boost::json::object concernedInfoObj
                = personInfo["concernedInfo"].as_array()[i].as_object();
            std::string imgId = concernedInfoObj["avatarId"].as_string().c_str();
            std::string avatarPath = "image://photos/" + imgId;
            std::string data = base64_decode(concernedInfoObj["avatar"].as_string().c_str());
            QPixmap pixmap;
            pixmap.loadFromData(reinterpret_cast<const unsigned char *>(data.c_str()),
                                static_cast<int>(data.length()));

            interestAvatorVec.insert(QString::fromStdString(imgId), pixmap);

            // 插入数据到 QJsonObject
            interest_Obj.insert("avatarPath", QString::fromStdString(avatarPath));
            interest_Obj.insert("file", "qrc:/qml/qml/PersonalPage.qml");

            // 添加到数组中
            arrayIn.append(QJsonValue(interest_Obj));
        }

        // 设置图片哈希表
        imageProvider->setInterestHashPixmaps(interestAvatorVec);
    }

    // 序列化 QJsonArray 为字符串
    QJsonDocument interest_doc(arrayIn);
    m_interest = QString(interest_doc.toJson(QJsonDocument::Indented));
    qDebug() << m_interest;

    // 粉丝信息
    QJsonArray fansArray = rootObj["fansInfo"].toArray();
    QJsonArray arrayFan;
    QHash<QString, QPixmap> fanAvatorVec;

    for (int i = 0; i < fansArray.size(); i++) {
        QJsonObject fan_Obj = fansArray[i].toObject();

        // 提取头像信息
        boost::json::object fansInfoObj = personInfo["fansInfo"].as_array()[i].as_object();
        std::string imgId = fansInfoObj["avatarId"].as_string().c_str();
        std::string avatarPath = "image://photos/" + imgId;
        std::string data = base64_decode(fansInfoObj["avatar"].as_string().c_str());
        QPixmap pixmap;
        pixmap.loadFromData(reinterpret_cast<const unsigned char *>(data.c_str()),
                            static_cast<int>(data.length()));

        fanAvatorVec.insert(QString::fromStdString(imgId), pixmap);

        // 插入数据到 QJsonObject
        fan_Obj.insert("avatarPath", QString::fromStdString(avatarPath));
        fan_Obj.insert("file", "qrc:/qml/qml/PersonalPage.qml");

        // 添加到数组中
        arrayFan.append(QJsonValue(fan_Obj));
    }

    // 设置图片哈希表
    imageProvider->setFanHashPixmaps(fanAvatorVec);

    // 序列化 QJsonArray 为字符串
    QJsonDocument fan_doc(arrayFan);
    m_fans = QString(fan_doc.toJson(QJsonDocument::Indented));

    if (m_fans.isEmpty()) {
        m_fans = "";
    }
}

//获取某一篇笔记的详细信息
void PersonalUIControl::getOneJottingDetail(QString jotting_id)
{
    qDebug() << "进来函数： PersonalUIControl::getOneJottingDetail============================";
    qDebug() << "获取个人页面某篇笔记的详细信息";

    std::string id = jotting_id.toStdString();

    // 创建 JSON 对象并赋值
    boost::json::object message;
    message["id"] = ID;
    message["jottingId"] = id;
    message["request"] = "GetInfoJottingDetail";

    // 序列化 JSON 对象
    std::string sendData = boost::json::serialize(message);

    // 发送数据
    Client::getInstance()->reconnect();
    Client::getInstance()->send(sendData.c_str(), sendData.length());

    std::cout << "Client"
              << "客户端获取用户推送个人页面笔记的详细信息......" << std::endl;

    // 接收数据
    char *receiveData = new char[10000000];
    Client::getInstance()->receive(receiveData);

    // 将接收到的数据转换为 std::string
    std::string json_str(receiveData);
    delete[] receiveData;
    //std::cout << json_str << std::endl;

    // 解析 JSON 字符串为 boost::json::value
    boost::json::error_code ec;
    boost::json::value parsed = boost::json::parse(json_str, ec);

    if (ec) {
        qDebug() << "JSON parse error:" << QString::fromStdString(ec.message());
        return;
    }

    // 转换为 boost::json::object
    boost::json::object jottingInfo = parsed.as_object();

    // 创建用于存储笔记详细信息的 JSON 对象
    boost::json::object jotting;

    // 笔记作者昵称
    boost::json::object netizen = jottingInfo["netizen"].as_object();
    jotting["netizenName"] = netizen["nickName"].as_string().c_str();

    // 笔记内容
    jotting["content"] = jottingInfo["content"].as_string().c_str();

    // 笔记时间
    jotting["time"] = jottingInfo["time"].as_string().c_str();

    std::cout << "笔记作者头像信息" << std::endl;
    // 笔记作者头像信息
    std::string avatarId = netizen["avatarId"].as_string().c_str();
    std::string avatarPath = "image://photos/" + avatarId;
    std::string avatarData = base64_decode(netizen["avatar"].as_string().c_str());
    QPixmap pixmap;
    pixmap.loadFromData(reinterpret_cast<const unsigned char *>(avatarData.c_str()),
                        static_cast<int>(avatarData.length()));
    imageProvider->setAvatar(pixmap);
    jotting["avatar"] = avatarPath.c_str();
    m_avatar = QString::fromStdString(avatarPath);

    std::cout << "笔记素材信息" << std::endl;
    // 笔记素材信息
    boost::json::array materials = jottingInfo["materials"].as_array();
    QHash<QString, QPixmap> allPics;

    jotting["picPath"] = boost::json::array();
    for (const auto &material : materials) {
        // 转换为 boost::json::object
        boost::json::object materialObj = material.as_object();

        // 确定路径
        std::string picId = materialObj["picId"].as_string().c_str();
        std::string picNewPath = "image://photos/" + picId;

        // 图片解码
        QPixmap newpixmap;
        std::string picData = base64_decode(materialObj["picContent"].as_string().c_str());
        newpixmap.loadFromData(reinterpret_cast<const unsigned char *>(picData.c_str()),
                               static_cast<int>(picData.length()));

        // 添加到哈希表
        allPics.insert(QString::fromStdString(picId), newpixmap);

        // 构建图片路径并添加到 jotting 对象
        boost::json::object picPathObj;
        picPathObj["path"] = picNewPath.c_str();
        jotting["picPath"].as_array().emplace_back(picPathObj);
    }

    // 设置图片哈希表
    imageProvider->setpushUIPics(allPics);
    std::cout << "笔记图片加载完成" << std::endl;

    // 笔记评论信息
    QHash<QString, QPixmap> avatars;
    boost::json::array comments = jottingInfo["comment"].as_array();

    jotting["comment"] = boost::json::array();
    for (const auto &com : comments) {
        // 转换为 boost::json::object
        boost::json::object commentObj = com.as_object();

        // 评论者昵称
        boost::json::object commentNetizen = commentObj["netizen"].as_object();
        std::string commentNetizenName = commentNetizen["nickName"].as_string().c_str();
        std::string commentContent = commentObj["content"].as_string().c_str();
        std::string commentTime = commentObj["time"].as_string().c_str();

        // 评论者头像信息
        std::string commentAvatarId = commentNetizen["avatarId"].as_string().c_str();
        std::string commentAvatarPath = "image://photos/" + commentAvatarId;
        std::string commentAvatarData = base64_decode(commentNetizen["avatar"].as_string().c_str());
        QPixmap commentPixmap;
        commentPixmap.loadFromData(reinterpret_cast<const unsigned char *>(
                                       commentAvatarData.c_str()),
                                   static_cast<int>(commentAvatarData.length()));
        avatars.insert(QString::fromStdString(commentAvatarId), commentPixmap);
        std::string commentPicPath = commentAvatarPath.c_str();

        // 构建评论对象
        boost::json::object comment;
        comment["netizenName"] = commentNetizenName.c_str();
        comment["content"] = commentContent.c_str();
        comment["time"] = commentTime.c_str();
        comment["pic"] = commentPicPath.c_str();

        // 添加到 jotting 对象
        jotting["comment"].as_array().emplace_back(comment);
    }

    // 序列化 JSON 对象为字符串
    m_jotting = QString::fromStdString(boost::json::serialize(jotting));

    // 输出日志
    qDebug() << "==============笔记的详细信息：==============";
    qDebug() << m_jotting;
}

void PersonalUIControl::initClientInfo(QString id, QString ipaddr)
{
    std::cout<<id.toStdString()<<"\n"<<ipaddr.toStdString()<<endl;
    ID = id.toStdString();
    IPADDR  =ipaddr.toStdString();
}

#include "imageprovider.h"
#include "base64.h"
#include "client.h"
#include "clientInfo.h"
#include <boost/json.hpp>

ImageProvider::ImageProvider()
    :QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize)
    Q_UNUSED(size)
    if (size)   *size = m_avatar.size();
    if(id=="avatar"){
        qDebug()<<"头像";
        return m_avatar;
    } else if(m_firstPics.find(id) != m_firstPics.end()){
        return m_firstPics[id];
    } else if(m_interestAvatars.find(id) != m_interestAvatars.end()){
        return m_interestAvatars[id];
    } else if(m_fanAvatars.find(id) != m_fanAvatars.end()){
        return m_fanAvatars[id];
    } else if(m_pushUIAvatars.find(id) != m_pushUIAvatars.end()){
        return m_pushUIAvatars[id];
    } else if(m_pushUIPics.find(id) != m_pushUIPics.end()){
        return m_pushUIPics[id];
    }else if(m_detailUIAvatars .find(id) != m_detailUIAvatars.end()){
        return m_detailUIAvatars[id];
    }else if(m_messageUIAvatars .find(id) != m_messageUIAvatars.end()){
        return m_messageUIAvatars[id];
    }else if(m_videoAvatars .find(id) != m_videoAvatars.end()){
        return m_videoAvatars[id];
    }

    return QPixmap();

//    return getPicture(id.toStdString());
}

QPixmap ImageProvider::getPicture(std::string path)
{
    qDebug() << QString::fromStdString(path);

    // 创建 JSON 对象并赋值
    boost::json::object message;
    message["id"] = ID;
    message["request"] = "GetPicture";
    message["picPath"] = boost::json::value_from(path); // 使用 value_from 进行类型转换

    // 序列化 JSON 对象
    std::string mes = boost::json::serialize(message);

    // 发送数据
    Client::getInstance()->reconnect();
    Client::getInstance()->send(mes.c_str(), mes.size());

    // 接收数据
    char *pic = new char[9999999];
    Client::getInstance()->receive(pic);
    std::string photo = base64_decode(pic);
    delete[] pic;

    // 加载图片数据到 QPixmap
    QPixmap pixmap;
    pixmap.loadFromData(reinterpret_cast<const unsigned char *>(photo.c_str()),
                        static_cast<int>(photo.size()),
                        nullptr);

    return pixmap;
}


void ImageProvider::setAvatar(const QPixmap &newPixmap)
{
    m_avatar = newPixmap;
}

void ImageProvider::setHashPixmaps(const QHash<QString, QPixmap> &newPixmaps)
{
    m_firstPics = newPixmaps;
}

void ImageProvider::setInterestHashPixmaps(const QHash<QString, QPixmap> &newPixmaps)
{
    m_interestAvatars = newPixmaps;
}

void ImageProvider::setFanHashPixmaps(const QHash<QString, QPixmap> &newPixmaps)
{
    m_fanAvatars = newPixmaps;
}

void ImageProvider::setpushUIAvatars(const QHash<QString, QPixmap> &newPixmaps)
{
    m_pushUIAvatars = newPixmaps;
}

void ImageProvider::setMessageUIAvatars(const QHash<QString, QPixmap> &newPixmaps)
{
    m_messageUIAvatars = newPixmaps;
}

void ImageProvider::setpushUIPics(const QHash<QString, QPixmap> &newPixmaps)
{
    m_pushUIPics = newPixmaps;
}

void ImageProvider::setDetailUIAvatars(const QHash<QString, QPixmap> &newPixmaps)
{
    m_detailUIAvatars = newPixmaps;
}

void ImageProvider::setVideoAvatars(const QHash<QString, QPixmap> &newPixmaps)
{
    m_videoAvatars = newPixmaps;
}




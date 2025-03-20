#include "publishpagecontrol.h"
#include <QDebug>

#include <boost/json.hpp>

PublishPageControl::PublishPageControl(QObject *parent):
    QObject(parent)
{

}

void PublishPageControl::findLocalMaterial()
{
    boost::json::object path;
    path["path"]="qrc:/images/images/avatar.png";
    path["isSelected"]=false;
    path["order"]="";

    boost::json::array materials;
    for (int i = 0; i < 20; ++i) {
        materials.emplace_back(path);
    }
    boost::json::object localMaterial;
    localMaterial["materials"] = materials;

    m_localMaterial = QString::fromStdString(boost::json::serialize(localMaterial));
}

void PublishPageControl::publishJotting(QString paths, QString content, QString nickName, QString head_pic)
{
    QStringList pathList=paths.split(",");

    boost::json::object jotting;
    jotting["avatar"]=head_pic.toStdString();
    jotting["netizenName"]=nickName.toStdString();
    jotting["content"]=content.toStdString();
    jotting["likeCount"]="0";
    jotting["collectCount"]="0";
    jotting["commentCount"]="0";
    jotting["time"]="0000-00-00 00:00";

    jotting["picPath"] = boost::json::array();

    for(auto &path:pathList){
        boost::json::object picPath;
        picPath["path"]=path.toStdString();
        jotting["picPath"].as_array().emplace_back(picPath);
    }
    qDebug() << QString::fromStdString(boost::json::serialize(jotting));
}

void PublishPageControl::setLocalMaterial(const QString &localMaterial)
{
    m_localMaterial=localMaterial;
}

QString PublishPageControl::getLocalMaterial() const
{
    return m_localMaterial;
}

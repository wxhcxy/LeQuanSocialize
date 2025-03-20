/******************************************************************************
 * Copyright (C) 1997-2022 Free Software Foundation, Inc.
 * All right reserved. See COPYRIGHT for detailed Information.
 *
 * @file       scanandcheckjottingcontroller.cpp
 * @brief      XXXX Function
 *
 * @author     李梦雪<2304768518@qq.com> 梁淑贞<moonlsz@163.com> 张宝丹<1395089569@qq.com>
 * @date       2022/08/14
 * @history
 *****************************************************************************/
#include "scanandcheckjottingcontroller.h"

#include "netizen.h"
#include "netizenbroker.h"

#include "encodephoto.h"

ScanAndCheckJottingController *ScanAndCheckJottingController::m_instance = nullptr;

ScanAndCheckJottingController *ScanAndCheckJottingController::getInstance()
{
    if (m_instance == nullptr) {
        m_instance = new ScanAndCheckJottingController();
    }
    return m_instance;
}

boost::json::object ScanAndCheckJottingController::pushJottings(std::string netizenId)
{

    // //用测试代码
    // boost::json::array materials;

    // // 添加第一个 material 对象
    // boost::json::object material1;
    // material1["picId"] = "pic_0001";
    // material1["picContent"] = encodePhoto("/root/ShareBook/Picture/picture1.png");
    // materials.emplace_back(material1);

    // // 添加第二个 material 对象
    // boost::json::object material2;
    // material2["picId"] = "pic_0002";
    // material2["picContent"] = encodePhoto("/root/ShareBook/Picture/picture2.png");
    // materials.emplace_back(material2);

    // // 创建内部 JSON 对象 netizen
    // boost::json::object netizen;
    // netizen["nickName"] = "Baby";
    // netizen["avatarId"] = "avatar_12345";
    // netizen["avatar"] = "base64_encoded_avatar_data";

    // // 创建 JSON 对象
    // boost::json::object jotting;

    // // 设置字段
    // jotting["id"] = "000001";
    // jotting["netizen"] = netizen;
    // jotting["content"] = "今天也是美好的一天";
    // jotting["time"] = "2025-01-01 13:00:12";
    // jotting["materials"] = materials;

    // boost::json::array jottings;
    // jottings.emplace_back(jotting);

    // boost::json::object jottingRes;
    // jottingRes["nickName"] = "anke";
    // jottingRes["avatarId"] = "avatar_anke";
    // jottingRes["avatar"] = "base64_encoded_avatar_data_anke";
    // jottingRes["jottings"] = jottings;

    // return jottingRes;

    Netizen &netizen = NetizenBroker::getInstance()->findById(netizenId);
    std::cout << "查找到网民ScanAndCheckJottingController::pushJottings" << std::endl;
    return netizen.scanJottings();
}

boost::json::object ScanAndCheckJottingController::pushJottingDetial(std::string netizenId,
                                                                     std::string jottingId)
{
    Netizen &netizen = NetizenBroker::getInstance()->findById(netizenId);
    return netizen.checkOneJotting(jottingId);
}

boost::json::object ScanAndCheckJottingController::pushInfoJottingDetail(string netizenId,
                                                                         string jottingId)
{
    Netizen &netizen = NetizenBroker::getInstance()->findById(netizenId);
    return netizen.checkInfoOneJotting(jottingId);
}

boost::json::object ScanAndCheckJottingController::scanVideos(string netizenId)
{
    Netizen &netizen = NetizenBroker::getInstance()->findById(netizenId);
    return netizen.scanVideos();
}

ScanAndCheckJottingController::ScanAndCheckJottingController() {}

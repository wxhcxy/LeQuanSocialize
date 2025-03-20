#include "material.h"
#include "encodephoto.h"

#define TEST_TYPE "PATH0"

Material::Material(const std::string &tid, std::string jottingId)
    : MaterialInterface(tid)
    , _jottingProxy(jottingId)
{}

Material::Material(const std::string id, std::string jottingId, std::string path, int type)
    : MaterialInterface(id)
    , _jottingProxy(jottingId)
    , m_path(path)
{
    std::cout << "jottingId Material::Material: " << jottingId << std::endl;
    std::cout << "m_path Material::Material: " << m_path << std::endl;
    switch (type) {
    case 1:
        m_type = PICTURE;
        break;
    case 2:
        m_type = AUDIO;
        break;
    case 3:
        m_type = VIDEO;
        break;
    default:
        std::cerr << "Material type error" << std::endl;
        break;
    }
}

const std::string Material::jottingId() const
{
    return _jottingProxy.id();
}

boost::json::object Material::getAbstract()
{
    std::cout << "Material::getAbstract()" << std::endl;
    boost::json::object j;
    if (m_type == PICTURE) {
        j["picId"] = m_path;
        if (strcmp(TEST_TYPE, "PATH")) {
            j["picContent"] = encodePhoto(m_path);
            std::cout << "[picContent]: " << j["picContent"] << std::endl;
        } else {
            std::cout << "222[picContent]: " << j["picContent"] << std::endl;
            j["picContent"] = m_path;
        }
    }
    return j;
}

const std::string &Material::path() const
{
    return m_path;
}

const std::string Material::type() const
{
    return "1";
}

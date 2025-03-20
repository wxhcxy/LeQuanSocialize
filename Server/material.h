#ifndef MATERIAL_H
#define MATERIAL_H

#include "jottingproxy.h"
#include "materialinterface.h"

enum MaterialType { PICTURE = 1, VIDEO, AUDIO };

class Material : public MaterialInterface
{
public:
    explicit Material(const std::string &tid, std::string jottingId);
    explicit Material(const std::string id, std::string jottingId, std::string path, int type);
    virtual ~Material() {}

    const std::string jottingId() const;
    virtual boost::json::object getAbstract() override;
    const std::string &path() const;
    const std::string type() const;

private:
    JottingProxy _jottingProxy;
    std::string m_path;
    MaterialType m_type;
};

#endif // MATERIAL_H

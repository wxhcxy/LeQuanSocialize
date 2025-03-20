#ifndef MATERIALPROXY_H
#define MATERIALPROXY_H

#include "materialinterface.h"
#include <memory>

class Material;

class MaterialProxy : public MaterialInterface
{
public:
    explicit MaterialProxy(const std::string &tid);
    virtual ~MaterialProxy();
    virtual boost::json::object getAbstract() override;

private:
    Material *_material;
};

#endif // MATERIALPROXY_H

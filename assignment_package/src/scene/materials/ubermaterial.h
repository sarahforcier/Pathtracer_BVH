#pragma once
#include "material.h"

class UberMaterial : public Material
{
public:
    UberMaterial(const Color3f &Kr, const Color3f &Kt,
                 const Color3f &Kd, const Color3f &Ks,
                 float roughnessX, float roughnessY, float opacity, float indexOfRefraction,
                 const std::shared_ptr<QImage> &roughnessMap,
                 const std::shared_ptr<QImage> &textureMapSpec,
                 const std::shared_ptr<QImage> &textureMapRefl,
                 const std::shared_ptr<QImage> &textureMapTransmit,
                 const std::shared_ptr<QImage> &textureMap,
                 const std::shared_ptr<QImage> &normalMap)
        : Kr(Kr), Kt(Kt), Kd(Kd), Ks(Ks),
          roughnessX(roughnessX), roughnessY(roughnessY), opacity(opacity),
          roughnessMap(roughnessMap), indexOfRefraction(indexOfRefraction),
          textureMapRefl(textureMapRefl), textureMapSpec(textureMapSpec),
          textureMapTransmit(textureMapTransmit),
          textureMap(textureMap), normalMap(normalMap)
    {}

    void ProduceBSDF(Intersection *isect) const;


private:
    Color3f Kr;
    Color3f Kt;
    Color3f Kd;
    Color3f Ks;
    float roughnessX; float roughnessY;
    float indexOfRefraction; float opacity;
    std::shared_ptr<QImage> roughnessMap;
    std::shared_ptr<QImage> textureMapSpec;
    std::shared_ptr<QImage> textureMapRefl;
    std::shared_ptr<QImage> textureMapTransmit;
    std::shared_ptr<QImage> textureMap;
    std::shared_ptr<QImage> normalMap;
};

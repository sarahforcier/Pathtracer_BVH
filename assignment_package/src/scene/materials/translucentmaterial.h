#pragma once
#include "material.h"

class TranslucentMaterial : public Material
{
public:
    TranslucentMaterial(const Color3f &Kd, const Color3f &Ks,
                        float roughness, float indexOfRefraction,
                        const std::shared_ptr<QImage> &textureMapRefl,
                        const std::shared_ptr<QImage> &textureMapTransmit,
                        const std::shared_ptr<QImage> &roughnessMap,
                        const std::shared_ptr<QImage> &textureMap,
                        const std::shared_ptr<QImage> &normalMap)
        : Kd(Kd), Ks(Ks), roughness(roughness), textureMapRefl(textureMapRefl),
          textureMapTransmit(textureMapTransmit), roughnessMap(roughnessMap),
          textureMap(textureMap), normalMap(normalMap), indexOfRefraction(indexOfRefraction)
    {}

    void ProduceBSDF(Intersection *isect) const;

private:
    Color3f Ks;
    Color3f Kd;
    float roughness;
    float indexOfRefraction;
    std::shared_ptr<QImage> textureMapRefl;
    std::shared_ptr<QImage> textureMapTransmit;
    std::shared_ptr<QImage> roughnessMap;
    std::shared_ptr<QImage> textureMap;
    std::shared_ptr<QImage> normalMap;
};

#pragma once
#include "material.h"

class MetalMaterial : public Material
{
public:
    MetalMaterial(const Color3f &R,
                 float roughnessX, float roughnessY, float indexOfRefraction,
                 const std::shared_ptr<QImage> &roughnessMap,
                 const std::shared_ptr<QImage> &textureMap,
                 const std::shared_ptr<QImage> &normalMap)
        : R(R), roughnessX(roughnessX), roughnessY(roughnessY),
          roughnessMap(roughnessMap), indexOfRefraction(indexOfRefraction),
          textureMap(textureMap), normalMap(normalMap)
    {}

    void ProduceBSDF(Intersection *isect) const;


private:
    Color3f R;
    float roughnessX; float roughnessY;
    float indexOfRefraction;
    std::shared_ptr<QImage> roughnessMap;
    std::shared_ptr<QImage> textureMap;
    std::shared_ptr<QImage> normalMap;
};

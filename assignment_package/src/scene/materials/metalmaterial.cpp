#include "metalmaterial.h"
#include "fresnel.h"
#include "microfacet.h"
#include "microfacetbrdf.h"
#include "specularbrdf.h"


void MetalMaterial::ProduceBSDF(Intersection *isect) const
{
    isect->bsdf = std::make_shared<BSDF>(*isect);

    if(this->normalMap)
    {
        isect->bsdf->normal = isect->bsdf->tangentToWorld *  Material::GetImageColor(isect->uv, this->normalMap.get());
        Vector3f tangent, bitangent;
        CoordinateSystem(isect->bsdf->normal, &tangent, &bitangent);
        isect->bsdf->UpdateTangentSpaceMatrices(isect->bsdf->normal, tangent, bitangent);
    }

    // Perfectly specular metal
    if(roughnessX == 0.f && roughnessY == 0.f) {
        isect->bsdf->Add(new SpecularBRDF(Color3f(1.f), new FresnelConductor(Color3f(1.f), Color3f(indexOfRefraction), R)));
    } else {
        float roughX = (roughnessX == 0.f) ? ShadowEpsilon : roughnessX;
        float roughY = (roughnessY == 0.f) ? ShadowEpsilon : roughnessY;
        if(this->roughnessMap) {
            Color3f roughRGB = Material::GetImageColor(isect->uv, this->roughnessMap.get());
            roughX *= (0.299f * roughRGB.r + 0.587f * roughRGB.g + 0.114f * roughRGB.b);
            roughY *= (0.299f * roughRGB.r + 0.587f * roughRGB.g + 0.114f * roughRGB.b);
        }
//      rough = RoughnessToAlpha(rough);
        MicrofacetDistribution* distrib = new TrowbridgeReitzDistribution(roughX, roughY);
        isect->bsdf->Add(new MicrofacetBRDF(Color3f(1.f), distrib, new FresnelConductor(Color3f(1.f), Color3f(indexOfRefraction), R)));
    }
}

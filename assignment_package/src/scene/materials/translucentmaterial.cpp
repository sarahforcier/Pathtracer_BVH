#include "translucentmaterial.h"
#include "specularbrdf.h"
#include "specularbtdf.h"
#include "microfacet.h"
#include "microfacetbrdf.h"
#include "microfacetbtdf.h"
#include "lambertbrdf.h"
#include "lambertbtdf.h"

void TranslucentMaterial::ProduceBSDF(Intersection *isect) const
{
    isect->bsdf = std::make_shared<BSDF>(*isect);

    // diffuse
    Color3f color = Kd;
    if(this->textureMap)
    {
        color *= Material::GetImageColor(isect->uv, this->textureMap.get());
    }

    isect->bsdf->Add(new LambertBRDF(color));
    isect->bsdf->Add(new LambertBTDF(color));

    if(this->normalMap)
    {
        isect->bsdf->normal = isect->bsdf->tangentToWorld *  Material::GetImageColor(isect->uv, this->normalMap.get());
        Vector3f tangent, bitangent;
        CoordinateSystem(isect->bsdf->normal, &tangent, &bitangent);
        isect->bsdf->UpdateTangentSpaceMatrices(isect->bsdf->normal, tangent, bitangent);
    }

    // specular
    Color3f reflectColor = Ks;
    if(this->textureMapRefl)
    {
        reflectColor *= Material::GetImageColor(isect->uv, this->textureMapRefl.get());
    }
    Color3f transmitColor = Ks;
    if(this->textureMapTransmit)
    {
        transmitColor *= Material::GetImageColor(isect->uv, this->textureMapTransmit.get());
    }

    if(roughness == 0.f) {
        isect->bsdf->Add(new SpecularBRDF(reflectColor, new FresnelDielectric(1.f, indexOfRefraction)));
        isect->bsdf->Add(new SpecularBTDF(transmitColor, 1.f, indexOfRefraction, new FresnelDielectric(1.f, indexOfRefraction)));
    }
    else {
        float rough = roughness;
        if(this->roughnessMap) {
            Color3f roughRGB = Material::GetImageColor(isect->uv, this->roughnessMap.get());
            rough *= (0.299f * roughRGB.r + 0.587f * roughRGB.g + 0.114f * roughRGB.b);
        }
//        rough = RoughnessToAlpha(rough);
        MicrofacetDistribution* distrib1 = new TrowbridgeReitzDistribution(rough, rough);
        MicrofacetDistribution* distrib2 = new TrowbridgeReitzDistribution(rough, rough);
        isect->bsdf->Add(new MicrofacetBTDF(transmitColor, distrib1, new FresnelDielectric(1.f, indexOfRefraction), 1.f, indexOfRefraction));
        isect->bsdf->Add(new MicrofacetBRDF(reflectColor, distrib2, new FresnelDielectric(1.f, indexOfRefraction)));
    }
}

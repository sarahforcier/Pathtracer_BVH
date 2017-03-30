#include "ubermaterial.h"
#include "specularbtdf.h"
#include "microfacet.h"
#include "microfacetbrdf.h"
#include "specularbrdf.h"
#include "specularbtdf.h"
#include "lambertbrdf.h"

void UberMaterial::ProduceBSDF(Intersection *isect) const
{
    isect->bsdf = std::make_shared<BSDF>(*isect);

    // diffuse
    Color3f color = opacity * Kd;
    if(this->textureMap)
    {
        color *= Material::GetImageColor(isect->uv, this->textureMap.get());
    }

    isect->bsdf->Add(new LambertBRDF(color));

    if(this->normalMap)
    {
        isect->bsdf->normal = isect->bsdf->tangentToWorld *  Material::GetImageColor(isect->uv, this->normalMap.get());
        Vector3f tangent, bitangent;
        CoordinateSystem(isect->bsdf->normal, &tangent, &bitangent);
        isect->bsdf->UpdateTangentSpaceMatrices(isect->bsdf->normal, tangent, bitangent);
    }

    // specular
    Color3f reflectColor = opacity * Kr;
    if(this->textureMapRefl)
    {
        reflectColor *= Material::GetImageColor(isect->uv, this->textureMapRefl.get());
    }
    Color3f transmitColor = opacity * Kt;
    if(this->textureMapTransmit)
    {
        transmitColor *= Material::GetImageColor(isect->uv, this->textureMapTransmit.get());
    }

    isect->bsdf->Add(new SpecularBRDF(reflectColor, new FresnelDielectric(1.f, indexOfRefraction)));
    isect->bsdf->Add(new SpecularBTDF(transmitColor, 1.f, indexOfRefraction, new FresnelDielectric(1.f, indexOfRefraction)));

    Color3f specularColor = opacity * Ks;
    if(this->textureMapSpec)
    {
        specularColor *= Material::GetImageColor(isect->uv, this->textureMapSpec.get());
    }
    if (roughnessX != 0.f || roughnessY != 0.f) {
        float roughX = (roughnessX == 0.f) ? ShadowEpsilon : roughnessX;
        float roughY = (roughnessY == 0.f) ? ShadowEpsilon : roughnessY;
        if(this->roughnessMap) {
            Color3f roughRGB = Material::GetImageColor(isect->uv, this->roughnessMap.get());
            roughX *= (0.299f * roughRGB.r + 0.587f * roughRGB.g + 0.114f * roughRGB.b);
            roughY *= (0.299f * roughRGB.r + 0.587f * roughRGB.g + 0.114f * roughRGB.b);
        }
//        rough = RoughnessToAlpha(rough);
        MicrofacetDistribution* distrib = new TrowbridgeReitzDistribution(roughX, roughY);
        isect->bsdf->Add(new MicrofacetBRDF(specularColor, distrib, new FresnelDielectric(1.f, indexOfRefraction)));
    }
}


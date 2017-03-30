#pragma once
#include "bsdf.h"
#include "fresnel.h"

class OrenNayar : public BxDF
{
public:
    OrenNayar(const Color3f &R, float sigma)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R) {
        sigma = Pi / 180.f * sigma;
        float sigma2 = sigma * sigma;
        A = 1.f - (sigma2 / (2.f * sigma2 + 0.33f));
        B = 0.45f * sigma2 / (sigma2 + 0.09f);
    }

    ~OrenNayar(){}

    Color3f f(const Vector3f &wo, const Vector3f &wi) const;

  private:
    const Color3f R;
    float A, B;
};

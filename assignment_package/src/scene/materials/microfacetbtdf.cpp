#include "microfacetbtdf.h"

Color3f MicrofacetBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO!
    if (SameHemisphere(wo, wi)) return Color3f(0.f);

    float cosO = AbsCosTheta(wo);
    float cosI = AbsCosTheta(wi);
    if (cosO == 0.f || cosI == 0.f) return Color3f(0.f);

    // inside or out?
    float n = CosTheta(wo) > 0.f ? etaB/etaA : etaA/etaB;

    Vector3f wh = glm::normalize(wo + n * wi);
    if (wh.z < 0.f) wh *= -1.f;

    float D = distribution->D(wh);
    float G = distribution->G(wo,wi);
    Color3f F = fresnel->Evaluate(glm::dot(wo,wh));
    float d = glm::dot(wo, wh) + n * glm::dot(wi, wh);
    return n * n * T * D * G * (Color3f(1.f) - F) * AbsDot(wi, wh) * AbsDot(wo, wh) /
            (d * d * cosO * cosI);
}

Color3f MicrofacetBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    //TODO
    if (wo.z == 0.f) return Color3f(0.f);
    if (sampledType) *sampledType = type;
    Vector3f wh = distribution->Sample_wh(wo, xi);

    // inside or out?
    float n = CosTheta(wo) > 0.f ? etaA/etaB : etaB/etaA;

    *wi = glm::refract(-wo, wh, n);
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

float MicrofacetBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    if (SameHemisphere(wo, wi)) return 0.f;

    // inside or out?
    float n = CosTheta(wo) > 0.f ? etaB/etaA : etaA/etaB;

    Vector3f wh = glm::normalize(wo + n * wi);

    if (wh.z < 0.f) wh = -wh;

    float d = glm::dot(wo, wh) + n * glm::dot(wi, wh);
    float dwh_dwi = glm::abs(n * n * glm::dot(wi,wh) / (d * d));
    return distribution->Pdf(wo, wh) * dwh_dwi;
}

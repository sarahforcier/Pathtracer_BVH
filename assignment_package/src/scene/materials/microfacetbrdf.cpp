#include "microfacetbrdf.h"

Color3f MicrofacetBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO!
    float cosO = AbsCosTheta(wo);
    float cosI = AbsCosTheta(wi);
    Vector3f wh = wo + wi;
    if (cosO == 0.f || cosI == 0.f) return Color3f(0.f);
    if (wh.x == 0.f && wh.y == 0.f && wh.z == 0.f) return Color3f(0.f);
    wh = glm::normalize(wh);
    float D = distribution->D(wh);
    float G = distribution->G(wo,wi);
    Color3f F = fresnel->Evaluate(glm::dot(wi,wh));
    return R * D * G * F / (4.f * cosO * cosI);
}

Color3f MicrofacetBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi, Float *pdf, BxDFType *sampledType) const
{
    //TODO
    if (wo.z == 0.f) return Color3f(0.f);
    if (sampledType) *sampledType = type;
    Vector3f wh = distribution->Sample_wh(wo, xi);
    *wi = glm::reflect(-wo,wh);
    if (!SameHemisphere(wo, *wi)) return Color3f(0.f);
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

float MicrofacetBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    Vector3f wh = glm::normalize(wo + wi);
    if (!SameHemisphere(wo, wi)) return 0.f;
    return distribution->Pdf(wo, wh) / (4.f * glm::dot(wo, wh));
}

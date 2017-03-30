#include "specularbTdf.h"

Color3f SpecularBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    return Color3f(0.f);
}


float SpecularBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return 0.f;
}

Color3f SpecularBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float *pdf, BxDFType *sampledType) const
{
    //TODO!
    if (sampledType) *sampledType = type;
    Color3f color(0.f);

    // inside or out?
    bool in_out = wo.z > 0;
    float n1 = in_out ? etaA : etaB;
    float n2 = in_out ? etaB : etaA;
    //Vector3f N = in_out ? Vector3f(0.f, 0.f, 1.f) : Vector3f(0.f, 0.f, -1.f);
    Vector3f N = Faceforward(Normal3f(0,0,1), wo);

    // ray direction
    float eta = n1/n2;
    float cosI = glm::dot(N, wo);
    float sin2I = std::max(0.f, 1.f - cosI * cosI);
    float sin2T = eta * eta * sin2I;
    if (sin2T >= 1.f) {
        return color;
    }// total internal reflection
    float cosT = std::sqrt(1.f - sin2T);
    *wi = eta * -wo + (eta * cosI - cosT) * N;

    *pdf = 1.f;
    color = T * (Color3f(1.f) - fresnel->Evaluate(wi->z))/std::abs(wi->z);
    return color;
}

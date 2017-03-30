#include "orennayar.h"

Color3f OrenNayar::f(const Vector3f &wo, const Vector3f &wi) const
{
    float sinI = SinTheta(wi);
    float sinO = SinTheta(wo);
     // cosine term
    float maxCos = 0;
    if (sinI > 0.0001f && sinO > 0.0001f) {
        float sinPhiI = SinPhi(wi), cosPhiI = CosPhi(wi);
        float sinPhiO = SinPhi(wo), cosPhiO = CosPhi(wo);
        float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
        maxCos = glm::max(0.f, dCos);
    }
    // sine and tangent terms
    float sinA, tanB;
    if (AbsCosTheta(wi) > AbsCosTheta(wo)) {
        sinA = sinO;
        tanB = sinI / AbsCosTheta(wi);
    } else {
        sinA = sinI;
        tanB = sinO / AbsCosTheta(wo);
    }
    return R * InvPi * (A + B * maxCos * sinA * tanB);
}

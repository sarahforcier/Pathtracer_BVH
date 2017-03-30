#include "fresnel.h"

Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    //TODO
    Color3f color(1.f);
    bool in_out = cosThetaI > 0.f;
    float nI = in_out ? etaI : etaT;
    float nT = in_out ? etaT : etaI;
    float cosI = in_out ? cosThetaI : std::abs(cosThetaI);
    float sinI = std::sqrt(std::max(0.f, 1 - cosI * cosI));
    float sinT = nI / nT * sinI;
    if (sinT >= 1.f) {
        return color;
    }
    float cosT = std::sqrt(std::max(0.f, 1 - sinT * sinT));
    float rPara = (nT * cosI - nI * cosT) / (nT * cosI + nI * cosT);
    float rPerp = (nI * cosI - nT * cosT) / (nI * cosI + nT * cosT);
    return Color3f((rPara * rPara + rPerp * rPerp) / 2.f);
}

Color3f FresnelConductor::Evaluate(float cosThetaI) const
{
   float cosI = glm::clamp(cosThetaI, -1.f, 1.f);
   Color3f eT = etaT / etaI;
   Color3f eK = k / etaI;

   float cosI2 = cosI * cosI;
   float sinI2 = 1 - cosI2;

   Color3f t0 = eT*eT - eK*eK - (1 - cosI2);
   Color3f a_b2 = t0*t0 + 4.f*eT*eK;
   Color3f a_b = Color3f(glm::sqrt(a_b2.x), glm::sqrt(a_b2.y),glm::sqrt(a_b2.z));
   Color3f t1 = a_b + cosI2;
   Color3f a2 = 0.5f * (a_b + t0);
   Color3f a = Color3f(glm::sqrt(a2.x), glm::sqrt(a2.y),glm::sqrt(a2.z));
   Color3f t2 = 2.f * cosI * a;
   Color3f Rs = (t1 - t2) / (t1 + t2);
   Color3f t3 = cosI2 * a_b + sinI2 * sinI2;
   Color3f t4 = t2 * sinI2;
   Color3f Rp = Rs * (t3 - t4) / (t3 + t4);

   return 0.5f * (Rp + Rs);
}

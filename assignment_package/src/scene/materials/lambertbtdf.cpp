#include "lambertbtdf.h"
#include <warpfunctions.h>

Color3f LambertBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return T * InvPi;
}

Color3f LambertBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //TODO
    if (sampledType) *sampledType = type;
    *wi = WarpFunctions::squareToHemisphereCosine(u);
    if (wo.z > 0.f) wi->z *= -1;
    *pdf = Pdf(wo, *wi);
    return f(wo,*wi);
}

float LambertBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO
    return !SameHemisphere(wo,wi) ? WarpFunctions::squareToHemisphereCosinePDF(wi):0;
}

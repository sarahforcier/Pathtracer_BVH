#include "bsdf.h"
#include <warpfunctions.h>

BSDF::BSDF(const Intersection& isect, float eta /*= 1*/)
    : worldToTangent(),
      tangentToWorld(),
      normal(isect.normalGeometric),
      eta(eta),
      numBxDFs(0),
      bxdfs{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
{
    UpdateTangentSpaceMatrices(normal, isect.tangent, isect.bitangent);
}

BSDF::~BSDF() {
    for(int i = 0; i < numBxDFs; i ++) {
        delete bxdfs[i];
    }
}

void BSDF::UpdateTangentSpaceMatrices(const Normal3f& n, const Vector3f& t, const Vector3f b)
{
    tangentToWorld = Matrix3x3(t, b, n);
    worldToTangent = glm::transpose(tangentToWorld);
}

Color3f BSDF::f(const Vector3f &woW, const Vector3f &wiW, BxDFType flags /*= BSDF_ALL*/) const
{
    Color3f sum = Color3f(0.f);
    Vector3f woT = worldToTangent * woW;
    Vector3f wiT = worldToTangent * wiW;
    for (int i = 0; i < numBxDFs; i++) {
        if (bxdfs[i]->MatchesFlags(flags)) {
            sum += bxdfs[i]->f(woT, wiT);
        }
    }
    return sum;
}

Color3f BSDF::Sample_f(const Vector3f &woW, Vector3f *wiW, const Point2f &xi,
                       float *pdf, BxDFType type, BxDFType *sampledType) const
{
    // Use the input random number _xi_ to select
    // one of our BxDFs that matches the _type_ flags.
    std::vector<BxDF*> arr;
    for (int i = 0; i < numBxDFs; i++) {
        if (bxdfs[i]->MatchesFlags(type)) arr.push_back(bxdfs[i]);
    }
    int index = int(xi.x * arr.size());
    BxDF* selection = bxdfs[index];

    // rewrite the random number contained in _xi_ to another number
    Point2f u = Point2f(xi.x * numBxDFs - index, xi.y);

    // Convert woW and wiW into tangent space and pass them to
    // the chosen BxDF's Sample_f (along with pdf).
    // Store the color returned by BxDF::Sample_f and convert
    // the _wi_ obtained from this function back into world space.
    Vector3f woT = worldToTangent * woW;
    Vector3f wiT;
    *pdf = 0.f;
    Color3f color = selection->Sample_f(woT,&wiT,u,pdf,sampledType);
    *wiW = glm::normalize(tangentToWorld * wiT);

    if (selection->type | BSDF_SPECULAR) {
        *pdf /= arr.size();
        return color;
    }
    // Iterate over all BxDFs
    *pdf = Pdf(woW, *wiW, type);
    color = f(woW, *wiW, type);
    return color;
}


float BSDF::Pdf(const Vector3f &woW, const Vector3f &wiW, BxDFType flags) const
{
    int num = 0;
    float sum = 0.f;
    Vector3f woT = worldToTangent * woW;
    Vector3f wiT = worldToTangent * wiW;
    for (int i = 0; i < numBxDFs; i++) {
        if (bxdfs[i]->MatchesFlags(flags)) {
            num++;
            sum += bxdfs[i]->Pdf(woT, wiT);
        }
    }
    return sum/num;
}

Color3f BxDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &xi,
                       Float *pdf, BxDFType *sampledType) const
{
    if (sampledType) *sampledType = type;
    Vector3f w = WarpFunctions::squareToHemisphereUniform(xi);
    *wi = w;
    *pdf = Pdf(wo, w);
    return f(wo,w);
}

// The PDF for uniform hemisphere sampling
float BxDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return SameHemisphere(wo, wi) ? Inv2Pi : 0;
}

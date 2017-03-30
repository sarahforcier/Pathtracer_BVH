#include "shape.h"
#include <QDateTime>

pcg32 Drawable::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());


void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}

Intersection Shape::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    //TODO
    // invoke two-input sample
    Float p;
    Intersection isect = Sample(xi, &p);
    // generate wi from resulting intersection
    Vector3f wi = glm::normalize(isect.point - ref.point);
    // convert to pdf with respect to solid angle
    *pdf = p * glm::distance2(ref.point, isect.point) / AbsDot(isect.normalGeometric, wi);
    return isect;
}

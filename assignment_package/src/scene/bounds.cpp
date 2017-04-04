#include "bounds.h"

bool Bounds3f::Intersect(const Ray &r, float* t) const
{
    //TODO
    Vector3f invDir = Vector3f(1.f/r.direction.x, 1.f/r.direction.y, 1.f/r.direction.z);
    int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};
    return IntersectP(r, invDir, dirIsNeg, t);
}

Bounds3f Bounds3f::Apply(const Transform &tr)
{
    //TODO
    float minX, maxX, minY, maxY, minZ, maxZ;
    Point4f p000 = tr.T() * Point4f(min, 1.f);
    Point4f p001 = tr.T() * Point4f(max.x, min.y, min.z, 1.f);
    Point4f p010 = tr.T() * Point4f(min.x, max.y, min.z, 1.f);
    Point4f p011 = tr.T() * Point4f(max.x, max.y, min.z, 1.f);
    Point4f p100 = tr.T() * Point4f(min.x, min.y, max.z, 1.f);
    Point4f p101 = tr.T() * Point4f(max.x, min.y, max.z, 1.f);
    Point4f p110 = tr.T() * Point4f(max.x, max.y, min.z, 1.f);
    Point4f p111 = tr.T() * Point4f(max, 1.f);
    minX = glm::min(p000.x, glm::min(p001.x, glm::min(p010.x, glm::min(p011.x,
           glm::min(p100.x, glm::min(p101.x, glm::min(p110.x, p111.x)))))));
    maxX = glm::max(p000.x, glm::max(p001.x, glm::max(p010.x, glm::max(p011.x,
           glm::max(p100.x, glm::max(p101.x, glm::max(p110.x, p111.x)))))));
    minY = glm::min(p000.y, glm::min(p001.y, glm::min(p010.y, glm::min(p011.y,
           glm::min(p100.y, glm::min(p101.y, glm::min(p110.y, p111.y)))))));
    maxY = glm::max(p000.y, glm::max(p001.y, glm::max(p010.y, glm::max(p011.y,
           glm::max(p100.y, glm::max(p101.y, glm::max(p110.y, p111.y)))))));
    minZ = glm::min(p000.z, glm::min(p001.z, glm::min(p010.z, glm::min(p011.z,
           glm::min(p100.z, glm::min(p101.z, glm::min(p110.z, p111.z)))))));
    maxZ = glm::max(p000.z, glm::max(p001.z, glm::max(p010.z, glm::max(p011.z,
           glm::max(p100.z, glm::max(p101.z, glm::max(p110.z, p111.z)))))));
    min = Point3f(minX, minY, minZ); max = Point3f(maxX, maxY, maxZ);
    return Bounds3f(min, max);
}

float Bounds3f::SurfaceArea() const
{
    Vector3f d = Diagonal();
    return 2.f * d.x * d.y + 2.f * d.y * d.z + 2.f * d.x * d.z;
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2)
{
    return Bounds3f(Point3f(std::min(b1.min.x, b2.min.x),
                            std::min(b1.min.y, b2.min.y),
                            std::min(b1.min.z, b2.min.z)),
                    Point3f(std::max(b1.max.x, b2.max.x),
                            std::max(b1.max.y, b2.max.y),
                            std::max(b1.max.z, b2.max.z)));
}

Bounds3f Union(const Bounds3f& b1, const Point3f& p)
{
    return Bounds3f(Point3f(std::min(b1.min.x, p.x),
                            std::min(b1.min.y, p.y),
                            std::min(b1.min.z, p.z)),
                    Point3f(std::max(b1.max.x, p.x),
                            std::max(b1.max.y, p.y),
                            std::max(b1.max.z, p.z)));
}

Bounds3f Union(const Bounds3f& b1, const glm::vec4& p)
{
    return Union(b1, Point3f(p));
}

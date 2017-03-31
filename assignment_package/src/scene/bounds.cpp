#include "bounds.h"

bool Bounds3f::Intersect(const Ray &r, float* t) const
{
    //TODO
    Vector3f invDir = Vector3f(1.f/r.direction.x, 1.f/r.direction.y, 1.f/r.direction.z);
    int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};
    return IntersectP(r, invDir, dirIsNeg);
}

Bounds3f Bounds3f::Apply(const Transform &tr)
{
    //TODO
    if (tr.isRotated()) {
        Point3f p000 = tr.T3() * min;
        Point3f p001 = tr.T3() * Point3f(max.x, min.y, min.z);
        Point3f p010 = tr.T3() * Point3f(min.x, max.y, min.z);
        Point3f p011 = tr.T3() * Point3f(max.x, max.y, min.z);
        Point3f p100 = tr.T3() * Point3f(min.x, min.y, max.z);
        Point3f p101 = tr.T3() * Point3f(max.x, min.y, max.z);
        Point3f p110 = tr.T3() * Point3f(max.x, max.y, min.z);
        Point3f p111 = tr.T3() * max;
        min = glm::min(p000, glm::min(p001, glm::min(p010, glm::min(p011,
              glm::min(p100, glm::min(p101, glm::min(p110, p111)))))));
        max = glm::max(p000, glm::max(p001, glm::max(p010, glm::max(p011,
              glm::max(p100, glm::max(p101, glm::max(p110, p111)))))));
        return Bounds3f(min, max);
    } else {
        min = tr.T3() * min;
        max = tr.T3() * max;
        return Bounds3f(min, max);
    }
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

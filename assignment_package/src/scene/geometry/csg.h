#pragma once

#include "shape.h"
#include "primitive.h"

struct CSGNode;
enum operation { OBJECT, UNION , INTER, DIFFER };

class CSG : public Primitive
{
public:
    CSG();
    ~CSG();
    Bounds3f WorldBound() const;
    bool Intersect(const Ray &ray, Intersection *isect) const;

private:
    //Members
    std::vector<std::shared_ptr<Primitive>> primitives;
    CSGNode *root = nullptr;
};

#include "naiveintegrator.h"

Color3f NaiveIntegrator::Li(Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    //TODO
    Intersection isect;
    Color3f Le;
    Color3f color;
    if (scene.Intersect(ray, &isect)) {
        Vector3f woW = - ray.direction;
        Le = isect.Le(woW);
        if (depth < 1 || !isect.objectHit->GetMaterial()) {
            color = Le;
        } else {
            isect.ProduceBSDF();
            Vector3f wiW;
            Point2f xi = sampler->Get2D();
            float pdf;

            Color3f c = isect.bsdf->Sample_f(woW, &wiW, xi, &pdf);
            Ray r = isect.SpawnRay(glm::normalize(wiW));
            Color3f li = Li(r, scene, sampler, depth - 1);
            if (pdf < 0.f) {
                color = Le;
            }
            else color =  Le + c * li * AbsDot(wiW, isect.normalGeometric)/pdf;
        }
    }
    return color;
}

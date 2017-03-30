#include "fulllightingintegrator.h"

Color3f FullLightingIntegrator::Li(Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth) const
{
    //TODO
    Color3f energy = Color3f(1.f);
    Color3f color = Color3f(0.f);
    // declarations
    float pdf; Vector3f wiW;
    BxDFType type = BSDF_DIFFUSE;
    BxDFType all = BSDF_ALL;
    int recursionLimit = depth;

    // number of lights
    int num= scene.lights.length();
    if (num == 0) return Color3f(0.f);

    while (depth > 0) {
        Intersection isect;
        if (!scene.Intersect(ray, &isect)) break;
        Vector3f woW = - ray.direction;
        if (!isect.objectHit->GetMaterial()) {
            if ((BSDF_SPECULAR & type) != 0 || depth == recursionLimit) color += energy * isect.Le(woW);
            break;
        }

        isect.ProduceBSDF();

        // light
        int index = std::min((int)(sampler->Get1D() * num), num - 1);
        const std::shared_ptr<Light> &light = scene.lights[index];
        Color3f li2 = light->Sample_Li(isect, sampler->Get2D(), &wiW, &pdf);
        Color3f f2 = isect.bsdf->f(woW, wiW);
        float wg = PowerHeuristic(1, pdf, 1, isect.bsdf->Pdf(woW, wiW));
        Color3f gColor(0.f);
        Intersection shad_Feel;
        if (scene.Intersect(isect.SpawnRay(wiW), &shad_Feel)) {
            if (pdf > 0.f && shad_Feel.objectHit->areaLight == scene.lights[index])
                gColor = f2 * li2 * AbsDot(wiW, isect.normalGeometric)/pdf;
        }
        // bsdf
        Color3f f1 = isect.bsdf->Sample_f(woW, &wiW, sampler->Get2D(), &pdf);
        Color3f fColor(0.f);
        Intersection isect_Test;
        if (pdf > 0.f && scene.Intersect(isect.SpawnRay(glm::normalize(wiW)), &isect_Test)) {
            if (isect_Test.objectHit->areaLight == scene.lights[index])
                fColor = light->L(isect_Test, -wiW) * f1 * AbsDot(wiW, isect.normalGeometric)/pdf;
        }
        float wf = PowerHeuristic(1, pdf, 1, light->Pdf_Li(isect, wiW));
        Color3f d_color = float(num) * (wf * fColor + wg * gColor);

        color += energy * d_color;

        // global illumination
        Color3f f0 = isect.bsdf->Sample_f(woW, &wiW, sampler->Get2D(), &pdf, all, &type);
        if (pdf > 0.f) {
            Color3f f = f0 * AbsDot(wiW, isect.normalGeometric)/pdf;
            energy *= f;
        }

        // russian roulette termination check
        float q = sampler->Get1D();
        float E = glm::max(energy.x, glm::max(energy.y, energy.z));
        if (depth < 3) {
            if (E < q) break;
            energy /= (1 - q);
        }

        // loop updates
        depth--;
        ray = isect.SpawnRay(wiW);
    }
    return color;
}

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    if (fPdf == 0.f && gPdf == 0.f) return 0.f;
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    //TODO
    if (fPdf == 0.f && gPdf == 0.f) return 0.f;
    float f = nf * fPdf;
    float g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}


#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>
#include "globals.h"

Point3f WarpFunctions::squareToDiskUniform(const Point2f &sample)
{
    float r = glm::sqrt(sample.x);
    float phi = TwoPi * sample.y;
    return glm::vec3(r * glm::cos(phi), r * glm::sin(phi), 0.f);
}

Point3f WarpFunctions::squareToDiskConcentric(const Point2f &sample)
{
    float r, phi;
    float a = 2.f * sample.x - 1.f; // map to [-1, 1]
    float b = 2.f * sample.y - 1.f;

    if (a > -b) {
        if (a > b) {
            r = a;
            phi = PiOver4 * b/a;
        } else {
            r = b;
            phi = PiOver4 * (2.f - a/b);
        }
    } else {
        if (a < b) {
            r = -a;
            phi = PiOver4 * (4.f + b/a);
        } else {
            r = -b;
            if (b != 0.f) phi = PiOver4 * (6.f - a/b);
            else phi = 0.f;
        }
    }
    return Point3f(r * glm::cos(phi), r * glm::sin(phi), 0.f);
}

float WarpFunctions::squareToDiskPDF(const Point3f &sample)
{
    return InvPi;
}

Point3f WarpFunctions::squareToSphereUniform(const Point2f &sample)
{
    float z = 1.f - 2.f * sample.x;
    float r = glm::sqrt(glm::max(0.f, 1.f - z * z));
    float phi = TwoPi * sample.y;
    return Point3f(r * glm::cos(phi), r * glm::sin(phi), z);
}

float WarpFunctions::squareToSphereUniformPDF(const Point3f &sample)
{
    return Inv4Pi;
}

Point3f WarpFunctions::squareToSphereCapUniform(const Point2f &sample, float thetaMin)
{
    float z = 1.f - 2.f * sample.x * (180 - thetaMin) / 180;
    float r = glm::sqrt(glm::max(0.f, 1.f - z * z));
    float phi = TwoPi * sample.y;
    return Point3f(r * glm::cos(phi), r * glm::sin(phi), z);
}

float WarpFunctions::squareToSphereCapUniformPDF(const Point3f &sample, float thetaMin)
{
    return Inv2Pi / (1 - glm::cos((180 - thetaMin)/180 * Pi));
}

Point3f WarpFunctions::squareToHemisphereUniform(const Point2f &sample)
{
    float r = glm::sqrt(glm::max(0.f, 1.f - sample.x * sample.x));
    float phi = TwoPi * sample.y;
    return Point3f(r * glm::cos(phi), r * glm::sin(phi), sample.x);
}

float WarpFunctions::squareToHemisphereUniformPDF(const Point3f &sample)
{
    return Inv2Pi;
}

Point3f WarpFunctions::squareToHemisphereCosine(const Point2f &sample)
{
    Point3f ret = squareToDiskConcentric(sample);
    ret.z = glm::sqrt(glm::max(0.f, 1.f - ret.x * ret.x - ret.y * ret.y));
    return ret;
}

float WarpFunctions::squareToHemisphereCosinePDF(const Point3f &sample)
{
    float x = sample.x; float y = sample.y; float z = sample.z;
    float r = glm::sqrt(x*x + y*y + z*z);
    float costheta = z/r;
    return costheta * InvPi;
}

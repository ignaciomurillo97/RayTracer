#ifndef H_SPHERE
#define H_SPHERE
#include "../vector.h"

typedef struct {
  double radius;
  Vector* center;
} Sphere;

double* raySphereIntersection(Ray r, void* s);
Vector sphereNormal (Vector point, void* obj);
#endif

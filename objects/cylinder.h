#ifndef H_CYLINDER
#define H_CYLINDER
#include "../vector.h"

typedef struct {
  double radius;
  double length;
  double cutSphereRadius;
  Vector* center;
  Vector* rotation;
} Cylinder;

double* rayCylinderIntersection(Ray r, void* obj);
Vector cylinderNormal (Vector point, void* obj);
#endif

#include "sphere.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

double* raySphereIntersection(Ray r, void* obj) {
  Sphere *s = (Sphere*)obj;
  double bx = r.direction->x * (r.origin->x - s->center->x);
  double by = r.direction->y * (r.origin->y - s->center->y);
  double bz = r.direction->z * (r.origin->z - s->center->z);
  double beta = 2 * (bx + by + bz);

  double gx = pow(r.origin->x - s->center->x, 2);
  double gy = pow(r.origin->y - s->center->y, 2);
  double gz = pow(r.origin->z - s->center->z, 2);
  double gamma = gx + gy + gz - pow(s->radius, 2);

  double disc = pow(beta, 2.0) - 4.0 * gamma;

  if (disc < 0) {
    double* result = (double*)malloc(sizeof(double));
    result[0] = 0;
    return result;
  }
  if (disc == 0) {
    double* result = (double*)malloc(sizeof(double) * 2);
    result[0] = 1;
    result[1] = (-beta) / 2.0; 
    return result;

  }
  double* result = (double*)malloc(sizeof(double) * 3);
  double t1 = (-beta - sqrt(disc)) / 2.0;
  double t2 = (-beta + sqrt(disc)) / 2.0;
  result[0] = 2;
  result[1] = t1;
  result[2] = t2;
  return result;
}

Vector sphereNormal (Vector point, void* obj) {
  Sphere* s = (Sphere*)obj;
  Vector v = subtractVector(point, *s->center);
  return normalize(v);
}

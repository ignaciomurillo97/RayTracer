#include "cylinder.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

double* rayCylinderIntersection(Ray r, void* obj) {
  Cylinder *c = (Cylinder*)obj;
  double alfa = pow(r.direction->x, 2) + pow(r.direction->z, 2);
  double beta = 2*r.origin->x*r.direction->x + 2*r.origin->z*r.direction->z;
  double gamma = pow(r.origin->x, 2) + pow(r.origin->z, 2) - pow(c->radius, 2);

  double disc = pow(beta, 2.0) - 4.0 * alfa * gamma;

  if (disc < 0) {
    double* result = (double*)malloc(sizeof(double));
    result[0] = 0;
    return result;
  }
  if (disc == 0) {
    double* result = (double*)malloc(sizeof(double) * 2);
    result[0] = 1;
    result[1] = (-beta) / (2.0*alfa); 
    return result;
  }

  double* result = (double*)malloc(sizeof(double) * 3);
  double t1 = (-beta - sqrt(disc)) / (2.0*alfa);
  double t2 = (-beta + sqrt(disc)) / (2.0*alfa);
  result[0] = 2;
  result[1] = t1;
  result[2] = t2;
  return result;
}

Vector cylinderNormal (Vector point, void* obj) {
  Cylinder *c = (Cylinder*)obj;
  Vector n = subtractVector(point, *c->center);
  n.y = 0;
  n = normalize(n);
  return n;
}

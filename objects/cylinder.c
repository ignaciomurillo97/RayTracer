#include "cylinder.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


double* rayCylinderIntersection(Ray r, void* obj) {
  Cylinder *c = (Cylinder*)obj;
  Vector origin = subtractVector(*r.origin, *c->center);
  Vector direction = *r.direction;
  direction = rotateX(direction, -c->rotation->x);
  direction = rotateY(direction, -c->rotation->y);
  direction = rotateZ(direction, -c->rotation->z);
  origin = rotateX(origin, -c->rotation->x);
  origin = rotateY(origin, -c->rotation->y);
  origin = rotateZ(origin, -c->rotation->z);
  double alfa = pow(direction.x, 2) + pow(direction.z, 2);
  double beta = 2*origin.x*direction.x + 2*origin.z*direction.z;
  double gamma = pow(origin.x, 2) + pow(origin.z, 2) - pow(c->radius, 2);

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

  int validPoints = 0;
  double* result = (double*)malloc(sizeof(double) * 3);
  double t1 = (-beta - sqrt(disc)) / (2.0*alfa);
  double t2 = (-beta + sqrt(disc)) / (2.0*alfa);
  double distT1 = distance(rayToPoint(r, t1), *c->center);
  double distT2 = distance(rayToPoint(r, t2), *c->center);
  if (distT1 < c->cutSphereRadius) {
    validPoints ++;
    result[validPoints] = t1;
  }
  if (distT2 < c->cutSphereRadius) {
    validPoints ++;
    result[validPoints] = t2;
  }
  result[0] = validPoints;
  return result;
}

Vector cylinderNormal (Vector point, void* obj) {
  Cylinder* c = (Cylinder*) obj;
  Vector pointFromCenter = subtractVector(point, *c->center);
  Vector CylinderDirection = (Vector) {0, 1, 0};
  CylinderDirection = rotateX(CylinderDirection, c->rotation->x);
  CylinderDirection = rotateY(CylinderDirection, c->rotation->y);
  CylinderDirection = rotateZ(CylinderDirection, c->rotation->z);
  double dp = dotProduct(CylinderDirection, pointFromCenter);
  Vector closestPoint = multVector (CylinderDirection, dp);
  Vector normal = normalize(subtractVector(point, closestPoint));
  printf("dp: %f - point:", dp);
  printVector(point);
  printf(" - closest Point:");
  printVector(closestPoint);
  printf(" - normal:");
  printVector(normal);
  printf("\n");
  return normal;
}

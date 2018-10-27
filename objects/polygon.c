#include "polygon.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

void calculatePolygonPlane(Polygon* p) {
  Vector* a = p->points[0];
  Vector* b = p->points[1];
  Vector* c = p->points[2];

  Vector ab = subtractVector(*b, *a);
  Vector ac = subtractVector(*c, *a);

  Vector polygonNormal = normalize(crossProduct(ab, ac));
  double D = -(
      polygonNormal.x * a->x +
      polygonNormal.y * a->y +
      polygonNormal.z * a->z
      );

  if (p->plane != NULL) free(p->plane);
  p->plane = (Plane*)malloc(sizeof(Plane));
  p->plane->A = polygonNormal.x;
  p->plane->B = polygonNormal.y;
  p->plane->C = polygonNormal.z;
  p->plane->D = D;
}

void flattenPolygon (Polygon* p) {
  int i;
  VectorUV** uvList = (VectorUV**) malloc (sizeof(VectorUV*) * p->pointCount);
  if (fabs(p->plane->A) > fabs(p->plane->B) && fabs(p->plane->A) > fabs(p->plane->C)) {
    // aplastar x
    for (i = 0; i < p->pointCount; i++) {
      uvList[i] = createVectorUV(p->points[i]->y, p->points[i]->z);
      p->droppedComponent = 0;
    }
  } else if (fabs(p->plane->B) > fabs(p->plane->C)) {
    // aplastar y
    for (i = 0; i < p->pointCount; i++) {
      uvList[i] = createVectorUV(p->points[i]->x, p->points[i]->z);
      p->droppedComponent = 1;
    }
  } else {
    // aplastar z
    for (i = 0; i < p->pointCount; i++) {
      uvList[i] = createVectorUV(p->points[i]->x, p->points[i]->y);
      p->droppedComponent = 2;
    }
  }

  p->flatPoints = uvList;
}

bool crossesXPositive (VectorUV a, VectorUV b) {
  if ((a.v > 0 && b.v > 0) || (a.v < 0 && b.v  < 0)) { // Rechazo trivial
    return false;
  }

  if (a.v == 0 && b.v == 0) {
    if (a.u > 0 || b.u > 0) {
      return true;
    }
    return false;
  }

  if (a.u >= 0 && b.u >= 0) { // Aceptacion trivial
    return true;
  }

  if (a.u < 0 && b.u < 0) { // Rechazo trivial
    return false;
  }

  // caso complejo
  double M = (a.u - b.u) / (a.v - b.v);
  double B = a.u - M * a.v;
  if (B >= 0) return true;
  return false;
}

bool inPolygon (double t, Ray r, Polygon* p) {
  Vector intersectionPoint = rayToPoint(r, t);
  VectorUV uvDisp = dropComponent(intersectionPoint, p->droppedComponent);
  VectorUV prev = *p->flatPoints[p->pointCount - 1];
  int count = 0;

  for (int i = 0; i < p->pointCount; i++) {
    VectorUV curr = *p->flatPoints[i];
    if (crossesXPositive(subtractVectorUV(prev, uvDisp), subtractVectorUV(curr, uvDisp))) {
      count ++;
    }
    prev = curr;
  }

  if (count % 2 == 1) {
    return true;
  } else {
    return false;
  }
}

double* rayPlaneIntersection(Ray r, void* p) {
  double* results;
  Polygon* polygon = (Polygon*) p;
  Plane* plane = polygon->plane;
  if (!(plane->A == 0 && plane->B == 0 && plane->C == 0)) {
    double t = -(
          plane->A * r.origin->x +
          plane->B * r.origin->y +
          plane->C * r.origin->z + plane->D
       ) / (
         plane->A * r.direction->x +
         plane->B * r.direction->y +
         plane->C * r.direction->z
         );

    if (inPolygon(t, r, p)) {
      results = (double*) malloc (sizeof(double) * 2);
      results[0] = 1;
      results[1] = t;
      return results;
    }
  }
  results = (double*) malloc (sizeof(double));
  results[0] = 0;
  return results;
}

Vector planeNormal (Vector point, void* obj) {
  Polygon* p = (Polygon*) obj;
  return (Vector) {p->plane->A, p->plane->B, p->plane->C};
}

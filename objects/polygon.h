#ifndef H_POLYGON
#define H_POLYGON
#include "../vector.h"

typedef struct {
  // Coeficientes
  double A;
  double B;
  double C;
  double D;
} Plane;

typedef struct {
  int pointCount;
  Vector** points;
  VectorUV** flatPoints;
  int droppedComponent;
  Plane* plane;
} Polygon;

double* rayPlaneIntersection(Ray r, void* p);
Vector planeNormal(Vector point, void* obj);
void calculatePolygonPlane(Polygon* p);
void flattenPolygon (Polygon* p);
bool crossesXPositive (VectorUV a, VectorUV b);

#endif

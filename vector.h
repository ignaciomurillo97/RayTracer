#ifndef H_VECTOR
#define H_VECTOR
#define VECTOR_ZERO (Vector) {0, 0, 0}
#include <stdbool.h>

typedef struct {
  double x;
  double y;
  double z;
} Vector;

typedef struct {
  double u;
  double v;
} VectorUV;

typedef struct {
  Vector* origin;
  Vector* direction;
} Ray;

Vector* createVector(double x, double y, double z);
VectorUV* createVectorUV(double u, double v);
VectorUV dropComponent(Vector v, int component);
double magnitude (Vector v);
double magnitudeSquared (Vector v);
double dotProduct (Vector v1, Vector v2);
Vector addVector (Vector v1, Vector v2);
Vector subtractVector (Vector v1, Vector v2);
VectorUV subtractVectorUV (VectorUV v1, VectorUV v2);
Vector multVector (Vector v, double scalar);
Vector normalize (Vector v);
void printVector(Vector v);
Vector rayToPoint(Ray r, double t);
bool colinearPoints(Vector a, Vector b, Vector c);
Vector crossProduct (Vector v1, Vector v2);
Vector reflectVector(Vector v, Vector ref);
double distance(Vector v1, Vector v2);
double distanceSquared(Vector v1, Vector v2);
Vector rotateX (Vector v, double radians);
Vector rotateY (Vector v, double radians);
Vector rotateZ (Vector v, double radians);
#endif

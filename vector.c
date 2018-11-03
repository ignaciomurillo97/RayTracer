#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

Vector* createVector(double x, double y, double z) {
  Vector* v = (Vector*)malloc(sizeof(Vector));
  v->x = x;
  v->y = y;
  v->z = z;
  return v;
}

VectorUV* createVectorUV(double u, double v) {
  VectorUV* vec = (VectorUV*)malloc(sizeof(VectorUV));
  vec->u = u;
  vec->v = v;
  return vec;
}

VectorUV dropComponent(Vector v, int component) {
  if (component == 0) {
    return (VectorUV) {v.y, v.z};
  }
  if (component == 1) {
    return (VectorUV) {v.x, v.z};
  }
  if (component == 2) {
    return (VectorUV) {v.x, v.y};
  }
}

Vector addVector (Vector v1, Vector v2) {
  return (Vector){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

Vector subtractVector (Vector v1, Vector v2) {
  return (Vector){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

VectorUV subtractVectorUV (VectorUV v1, VectorUV v2) {
  return (VectorUV){v1.u - v2.u, v1.v - v2.v};
}

Vector multVector (Vector v, double scalar) {
  return (Vector){v.x * scalar, v.y * scalar, v.z * scalar};
}

Vector divVector (Vector v, double scalar) {
  return (Vector){v.x / (double)scalar, v.y / (double)scalar, v.z / (double)scalar};
}

Vector normalize (Vector v) {
  return divVector(v, magnitude(v));
}

double magnitude (Vector v){
  return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

double magnitudeSquared (Vector v){
  return pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2);
}

double dotProduct (Vector v1, Vector v2) {
  return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

Vector crossProduct (Vector v1, Vector v2) {
  return (Vector){(v1.y * v2.z - v1.z * v2.y), (v2.x * v1.z - v1.x * v2.z), (v1.x * v2.y - v2.x * v1.y)};
}

void printVector(Vector v) {
  printf("(%f, %f, %f)", v.x, v.y, v.z);
}

Vector rayToPoint(Ray r, double t) {
  return addVector(*r.origin, multVector(*r.direction, t));
}

bool colinearPoints(Vector a, Vector b, Vector c) {
  double ab = magnitude(subtractVector(a, b));
  double bc = magnitude(subtractVector(b, c));
  double ac = magnitude(subtractVector(a, c));

  printf("%f, %f\n", ab + bc, ac);

  if (ab + bc == ac) return true;
  return false;
}

Vector reflectVector(Vector v, Vector ref) {
  double proyection = dotProduct(v, ref);
  Vector proyectedVector = multVector(ref, proyection);
  Vector offset = subtractVector(v, proyectedVector);
  offset = multVector(offset, -1);
  return addVector(proyectedVector, offset);
}

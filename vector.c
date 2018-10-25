#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

Vector addVector (Vector v1, Vector v2) {
  return (Vector){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

Vector subtractVector (Vector v1, Vector v2) {
  return (Vector){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
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

double dotProduct (Vector v1, Vector v2) {
  return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

void printVector(Vector v) {
  printf("(%f, %f, %f)", v.x, v.y, v.z);
}

Vector* createVector(double x, double y, double z) {
  Vector* v = (Vector*)malloc(sizeof(Vector));
  v->x = x;
  v->y = y;
  v->z = z;
}

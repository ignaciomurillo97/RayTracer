#ifndef H_VECTOR
#define H_VECTOR

typedef struct {
  double x;
  double y;
  double z;
} Vector;

typedef struct {
  Vector* origin;
  Vector* direction;
} Ray;

Vector* createVector(double x, double y, double z);
double magnitude (Vector v);
double dotProduct (Vector v1, Vector v2);
Vector addVector (Vector v1, Vector v2);
Vector subtractVector (Vector v1, Vector v2);
Vector multVector (Vector v, double scalar);
Vector normalize (Vector v);
void printVector(Vector v);

#endif

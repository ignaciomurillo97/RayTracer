#ifndef H_OBJECTS
#define H_OBJECTS
#include "../vector.h"
#include "../raytracing.h"

typedef struct {
  double bot;
  double left;
  double top;
  double right;
  int pixelWidth;
  int pixelHeight;
} Window;

enum ObjectType {SphereObject};

typedef struct RenderObject{
  void* object;
  double* (*intersectionFunction)(Ray, void*);
  Vector (*normalFunction)(Vector, void*);
  enum ObjectType type;
  struct RenderObject* next;
  Color color;
} RenderObject;

typedef struct {
  RenderObject *start;
  RenderObject *end;
} RenderList;

typedef struct {
  Vector* normal;
  Vector* contactPoint;
  RenderObject* object;
} Intersection;

typedef struct {
  Vector position;
  double intensity;
  double cConstant;
  double cLinear;
  double cCuadratic;
} Light;

void addToList (RenderList* list, RenderObject* obj);
Vector framebuffToUniverse (Window* w, int x, int y);
Vector rayToPoint(Ray r, double t);

#endif

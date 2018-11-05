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

enum ObjectType {
  SphereObject,
  PlaneObject
};

typedef struct RenderObject{
  void* object;
  double* (*intersectionFunction)(Ray, void*);
  Vector (*normalFunction)(Vector, void*);
  enum ObjectType type;
  struct RenderObject* next;
  Color color;
  double diffuseCoefficient;
  double specularCoefficient;
} RenderObject;

typedef struct {
  RenderObject *start;
  RenderObject *end;
} RenderList;

typedef struct {
  Vector* normal;
  Vector* contactPoint;
  double t;
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
Vector framebuffToUniverse (Window* w, double x, double y);
double attenuationFactor (Light light, double distToLight);

#endif

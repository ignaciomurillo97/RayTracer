#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector.h"
#include "objects/sphere.h"
#include "objects/objects.h"
#include "raytracing.h"
#include "renderEngine.h"
#include "file.h"

double smallestPositive(double* list) {
  int length = (int)list[0];
  if (length > 0) {
    double res = list[1];
    for (int i = 2; i <= length; i++) {
      if (list[i] < res && list[i] > 0) {
        res = list[i];
      }
    }
    if (res > 0) return res;
  }
  return -1;
} 

Intersection firstIntersection (Ray r, RenderList* renderList) {
  RenderObject* ro = renderList->start;
  double tMin = INFINITY;
  RenderObject* tMinObject = NULL;
  while (ro != NULL) {
    double* tList = (double*)(*(ro->intersectionFunction))(r, (void*)ro->object);
    double newTMin = smallestPositive(tList);
    if (newTMin != -1 && newTMin < tMin) {
      tMin = newTMin;
      tMinObject = ro;
    }
    ro = ro->next;
  }

  if (tMin != INFINITY) {
    Vector point = rayToPoint(r, tMin);
    Vector normal = (Vector)(*tMinObject->normalFunction)(point, tMinObject->object);
    return (Intersection){
      createVector(normal.x, normal.y, normal.z),
      createVector(point.x, point.y, point.z),
      tMinObject,
    };
  }
  return (Intersection){NULL, NULL};
}

Color colorMult(Color c, double scalar) {
  return (Color){c.r * scalar, c.g * scalar, c.b * scalar, c.a};
}

double crop (double n) {
  if (n < 0) {
    return 0;
  }
  if (n > 1) {
    return 1;
  }
  return n;
} 

double diffuseCoeficient(Light light, Intersection i) {
  Vector l = subtractVector(light.position, *i.contactPoint);
  double mag = magnitude(l);
  l = normalize(l);
  double coef = dotProduct(l, *i.normal) * diffuseCorrection;
  double fAtt = crop(1/ (light.cConstant + light.cLinear * mag + light.cCuadratic * pow(mag, 2)));
  coef *= fAtt;
  if (coef < 0) return 0;
  return coef;
}

Color whatColor(Ray r, RenderList* renderList, Light* lights, int lightCount) {
  double diffCoef = 0;

  Intersection intersection = firstIntersection(r, renderList);
  if (intersection.object != NULL) {
    Color objColor = intersection.object->color;
    for (int i = 0; i < lightCount; i++) {
      diffCoef += diffuseCoeficient(lights[i], intersection) * lights[i].intensity + bgIntensity;
    }
    diffCoef = crop(diffCoef);
    objColor = colorMult(objColor, diffCoef);
    return objColor;
  }
  return bgColor;
}

void render (Window* w, Vector* eye, Light* lights, int lightCount, Color** frameBuffer, RenderList* renderList) {
  int x, y;

  for ( y = 0; y < w->pixelHeight; y++ ) {
    for ( x = 0; x < w->pixelWidth; x++ ) {
      Vector v = framebuffToUniverse(w, x, y);
      Vector dir = subtractVector(v, *eye);
      dir = normalize(dir);
      Ray r = (Ray){ eye, &dir };
      Color c = whatColor(r, renderList, lights, lightCount);
      frameBuffer[x][w->pixelHeight - y - 1] = c;
    }
  }
}


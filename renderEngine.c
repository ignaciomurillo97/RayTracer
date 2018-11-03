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

Vector closestNormal (Vector direction, Vector normal) {
  double dotProd = dotProduct(direction, normal);
  if (dotProd < 0) {
    return normal;
  } else {
    return multVector(normal, -1);
  }
}

Intersection firstIntersection (Ray r, RenderList* renderList) {
  RenderObject* ro = renderList->start;
  double tMin = INFINITY;
  RenderObject* tMinObject = NULL;
  while (ro != NULL) {
    double* tList = (double*)(*(ro->intersectionFunction))(r, (void*)ro->object);
    double newTMin = smallestPositive(tList);
    if (newTMin != -1 && newTMin < tMin && newTMin > EPSILON) {
      tMin = newTMin;
      tMinObject = ro;
    }
    ro = ro->next;
  }

  if (tMin != INFINITY) {
    Vector point = rayToPoint(r, tMin);
    Vector normal = (Vector)(*tMinObject->normalFunction)(point, tMinObject->object);
    normal = closestNormal(*r.direction, normal);
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


Color colorAdd(Color c1, Color c2) {
  return (Color){c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, 1};
}

Color colorSubtract(Color c1, Color c2){
  return (Color){c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, 1};
}

Color colorApproach(Color base, Color target, double percentage) {
  return colorAdd(base, colorMult(colorSubtract(target, base), percentage));
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
  double distToLight = magnitude(l);
  l = normalize(l);
  double coef = dotProduct(l, *i.normal) * diffuseCorrection;
  double fAtt = crop(1/ attenuationFactor(light, distToLight));
  coef = coef * fAtt * light.intensity;
  if (coef < 0) return 0;
  return coef;
}

double specularCoeficient(Light light, Ray ray, Intersection i, double specularObjPercent) {
  Vector l = subtractVector(light.position, *i.contactPoint);
  double distToLight = magnitude(l);
  l = normalize(l);
  Vector dirInv = multVector(*ray.direction, -1);
  Vector reflectedVector = reflectVector(l, *i.normal);
  double coef = dotProduct(dirInv, reflectedVector);
  double fAtt = crop(1/ attenuationFactor(light, distToLight));
  if (coef < 0) return 0;
  coef = pow(coef, specularObjPercent);
  coef = coef * fAtt * light.intensity;
  return coef;
}

Color whatColor(Ray r, RenderList* renderList, LinkedList* lights, int lightCount) {
  double diffCoef = 0;
  double specCoef = 0;

  Intersection intersection = firstIntersection(r, renderList);
  if (intersection.object != NULL) {
    Color objColor = intersection.object->color;
    double specularObjPercent = intersection.object->specularCoefficient;

    Container* c = lights->start;
    while (c != NULL) {
      Light light = *(Light*)c->element;
      Vector l = subtractVector(light.position, *intersection.contactPoint);
      l = normalize(l);
      Ray rayToLight = (Ray) {intersection.contactPoint, &l};
      if (firstIntersection(rayToLight, renderList).object == NULL) {
        diffCoef += diffuseCoeficient(light, intersection);
        specCoef += specularCoeficient(light, r, intersection, specularObjPercent);
      }
      c = c->next;
    }

    diffCoef = crop(diffCoef) + bgIntensity;
    specCoef = crop(specCoef);
    objColor = colorMult(objColor, diffCoef);
    objColor = colorApproach(objColor, (Color){1, 1, 1, 1}, specCoef);
    return objColor;
  }
  return bgColor;
}

void render (Window* w, Vector* eye, LinkedList* lights, int lightCount, Color** frameBuffer, RenderList* renderList) {
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


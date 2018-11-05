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
    if (res > EPSILON) return res;
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
    if (newTMin != -1 && newTMin < tMin) {
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
      tMin,
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

bool ocluded(Light light, Intersection intersection, RenderList* renderList) {
  Vector l = subtractVector(light.position, *intersection.contactPoint);
  double distToLight = magnitude(l);
  l = normalize(l);
  Ray rayToLight = (Ray) {intersection.contactPoint, &l};
  Intersection occlusionObj = firstIntersection(rayToLight, renderList);
  return !(occlusionObj.object != NULL && occlusionObj.t > EPSILON && occlusionObj.t < distToLight);
}

Color whatColor(Ray r, RenderList* renderList, LinkedList* lights, int depth) {
  double diffCoef = 0;
  double specCoef = 0;

  Intersection intersection = firstIntersection(r, renderList);
  if (intersection.object != NULL) {
    Color objColor = intersection.object->color;
    double specularObjPercent = intersection.object->specularCoefficient;

    Container* c = lights->start;
    while (c != NULL) {
      Light light = *(Light*)c->element;

      if (ocluded(light, intersection, renderList)) {
        diffCoef += diffuseCoeficient(light, intersection);
        specCoef += specularCoeficient(light, r, intersection, specularObjPercent);
      }
      c = c->next;
    }

    diffCoef = crop(diffCoef) + bgIntensity;
    specCoef = crop(specCoef);
    objColor = colorMult(objColor, diffCoef);
    objColor = colorApproach(objColor, (Color){1, 1, 1, 1}, specCoef);

    // espejo 
    if (depth > 0) {
      Color mirrorColor;
      Vector eyeDirection = multVector(*r.direction, -1);
      Vector mirrorDirection = reflectVector(eyeDirection, *intersection.normal);
      Ray newRay = (Ray) { intersection.contactPoint, &mirrorDirection };
      mirrorColor = whatColor(newRay, renderList, lights, depth);
      objColor = colorApproach(objColor, mirrorColor, 0.4);
    }


    return objColor;
  }
  return bgColor;
}

void render (Window* w, Vector* eye, LinkedList* lights, Color** frameBuffer, RenderList* renderList) {
  int x, y;

  for ( y = 0; y < w->pixelHeight; y++ ) {
    for ( x = 0; x < w->pixelWidth; x++ ) {
      Color c;
      if (ANTIALIAS_ENABLED) {
        Vector v = framebuffToUniverse(w, x + 0.5, y + 0.5);
        Vector v1 = framebuffToUniverse(w, x, y);
        Vector v2 = framebuffToUniverse(w, x + 1, y);
        Vector v3 = framebuffToUniverse(w, x, y + 1);
        Vector v4 = framebuffToUniverse(w, x + 1, y + 1);

        Vector dir = normalize(subtractVector(v, *eye));
        Vector dir1 = normalize(subtractVector(v1, *eye));
        Vector dir2 = normalize(subtractVector(v2, *eye));
        Vector dir3 = normalize(subtractVector(v3, *eye));
        Vector dir4 = normalize(subtractVector(v4, *eye));

        Ray r = (Ray){ eye, &dir };
        Ray r1 = (Ray){ eye, &dir1 };
        Ray r2 = (Ray){ eye, &dir2 };
        Ray r3 = (Ray){ eye, &dir3 };
        Ray r4 = (Ray){ eye, &dir4 };

        Color c0 = whatColor(r, renderList, lights, 2);
        Color c1 = whatColor(r1, renderList, lights, 2);
        Color c2 = whatColor(r2, renderList, lights, 2);
        Color c3 = whatColor(r3, renderList, lights, 2);
        Color c4 = whatColor(r4, renderList, lights, 2);
        
        c = (Color) {
          (c0.r + c1.r + c2.r + c3.r + c4.r) / 5,
          (c0.g + c1.g + c2.g + c3.g + c4.g) / 5,
          (c0.b + c1.b + c2.b + c3.b + c4.b) / 5,
          1
        };

      } else {
        Vector v = framebuffToUniverse(w, x + 0.5, y + 0.5);
        Vector dir = subtractVector(v, *eye);
        dir = normalize(dir);
        Ray r = (Ray){ eye, &dir };
        c = whatColor(r, renderList, lights, 0);
      }
      frameBuffer[x][w->pixelHeight - y - 1] = c;
    }
  }
}


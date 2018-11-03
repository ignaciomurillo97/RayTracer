#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "vector.h"
#include "objects/sphere.h"
#include "objects/polygon.h"
#include "objects/objects.h"
#include "raytracing.h"
#include "renderEngine.h"
#include "Helpers/linkedlist.h"
#include "file.h"

Color** frameBuffer;
RenderList* renderList;

LinkedList *lights;
int lightCount;
Vector* eye;
Window* w;

void initializeFrameBuffer () {
  frameBuffer = (Color**)malloc(sizeof(Color*) * H_SIZE);
  for (int i = 0; i < H_SIZE; i++) {
    frameBuffer[i] = (Color*)malloc(sizeof(Color) * V_SIZE);
  }
}

void initializeWindow(Window* w) {
  w->bot = -22.5;
  w->left = -40;
  w->right = 40;
  w->top = 22.5;
  w->pixelWidth = H_SIZE;
  w->pixelHeight = V_SIZE;
}

void initializeEye(Vector* eye) {
  eye->x = 0;
  eye->y = 0;
  eye->z = -40;
}

Light* createLight(Vector pos, double intensity, double cConstant, double cLinear, double cCuadratic) {
  Light* l = (Light*)malloc(sizeof(Light));
  l->position = pos;
  l->intensity = intensity;
  l->cConstant = cConstant;
  l->cLinear = cLinear;
  l->cCuadratic = cCuadratic;
  return l;
}

void createAndPackSphere(double radius, Vector* center, RenderObject* ro, Color color) {
  Sphere* s = (Sphere*)malloc(sizeof(Sphere));
  s->radius = radius;
  s->center = center;
  ro->object = s;
  ro->intersectionFunction = &raySphereIntersection;
  ro->normalFunction = &sphereNormal;
  ro->type = SphereObject;
  ro->color = color;
}

void createAndPackPlane(int pointCount, Vector** points, RenderObject* ro, Color color) {
  Polygon* p = (Polygon*) malloc (sizeof(Polygon));
  p->pointCount = pointCount;
  p->points = points;
  p->plane = NULL;
  p->flatPoints = NULL;

  calculatePolygonPlane(p);
  flattenPolygon(p);

  ro->object = p;
  ro->intersectionFunction = &rayPlaneIntersection;
  ro->normalFunction = &planeNormal;
  ro->type = PlaneObject;
  ro->color = color;
}

int wildcmp(const char *wild, const char *string) {
  // Written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>
  const char *cp = NULL, *mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}

// TODO cambiar para usar scanf
void readFromFile(char* filename, RenderList* renderList) {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  RenderObject*  ro;

  fp = fopen(filename, "r");
  if (fp == NULL){
    exit(EXIT_FAILURE);
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    if (!wildcmp("//*", line)) {
      if (wildcmp("light*", line)) {
        double positionX;
        double positionY;
        double positionZ;
        double intensity;
        double constantComp;
        double linearComp;
        double cuadraticComp;

        int itemsRead = sscanf(line, "light (%lf, %lf, %lf) %lf (%lf, %lf, %lf)", &positionX,  &positionY,  &positionZ,  &intensity,  &constantComp,  &linearComp,  &cuadraticComp);
        if (itemsRead != 7) exit (EXIT_FAILURE);
        printf("items read: %d", itemsRead);
        addToLinkedList((void*) createLight((Vector){positionX, positionY, positionZ}, intensity, constantComp, linearComp, cuadraticComp), lights);
      } else if (wildcmp("sphere*", line)) {
        double radius;
        double centerX;
        double centerY;
        double centerZ;
        double colorR;
        double colorG;
        double colorB;

        int itemsRead = sscanf(line, "sphere %lf (%lf, %lf, %lf) (%lf, %lf, %lf)", &radius, &centerX, &centerY, &centerZ, &colorR, &colorG, &colorB);
        if (itemsRead != 7) exit (EXIT_FAILURE);
        ro = (RenderObject*)malloc(sizeof(RenderObject));
        createAndPackSphere(radius, createVector(centerX, centerY, centerZ), ro, (Color){colorR, colorG, colorB, 1});
        addToList(renderList, ro);
      }
    }
  }

  fclose(fp);
  if (line)
    free(line);
}

int main (int argc, char** argv) {
  if (argc != 3){
    printf("uso: raytracing.bin <archivo_entrada> <archivo_salida>");
    exit(EXIT_FAILURE);
  } 
  renderList = (RenderList*)malloc(sizeof(RenderList));
  eye = (Vector*)malloc(sizeof(Vector));
  w = (Window*)malloc(sizeof(Window));
  lights = (LinkedList*)malloc(sizeof(LinkedList));

  initializeFrameBuffer();
  initializeEye(eye);
  initializeWindow(w);
  readFromFile(argv[1], renderList); 

  render(w, eye, lights, lightCount, frameBuffer, renderList);

  frameBufferToFile(argv[2], frameBuffer, H_SIZE, V_SIZE);

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vector.h"
#include "objects/sphere.h"
#include "objects/polygon.h"
#include "objects/objects.h"
#include "raytracing.h"
#include "renderEngine.h"
#include "file.h"

Color** frameBuffer;
RenderList* renderList;

Light* lights;
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

Light createLight(Vector pos, double intensity, double cConstant, double cLinear, double cCuadratic) {
  return (Light) {pos, intensity, cConstant, cLinear, cCuadratic};
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

// TODO cambiar para usar scanf
Light* readFromFile(char* filename, RenderList* renderList) {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  RenderObject*  ro;
  Light* lights;
  int i = 0;

  fp = fopen(filename, "r");
  if (fp == NULL){
    exit(EXIT_FAILURE);
  }

  if ((read = getline(&line, &len, fp)) != -1) {
    char* lightCountChar = strtok(line, "\n");
    if (lightCountChar == NULL) exit(EXIT_FAILURE);
    lightCount = atoi(lightCountChar);
    if (lightCount == 0) exit(EXIT_FAILURE);
    lights = (Light*)malloc(sizeof(Light) * lightCount);
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    char* type = strtok(line, ",");
    if (type == NULL) exit(EXIT_FAILURE);

    if (strcmp(type, "S") == 0) {
      char* radiusChar = strtok(NULL, ",");
      if (radiusChar == NULL) exit(EXIT_FAILURE);
      double radius = strtod(radiusChar, NULL);

      char* centerXChar = strtok(NULL, ",");
      if (centerXChar == NULL) exit(EXIT_FAILURE);
      double centerX = strtod(centerXChar, NULL);
      char* centerYChar = strtok(NULL, ",");
      if (centerYChar == NULL) exit(EXIT_FAILURE);
      double centerY = strtod(centerYChar, NULL);
      char* centerZChar = strtok(NULL, ",");
      if (centerZChar == NULL) exit(EXIT_FAILURE);
      double centerZ = strtod(centerZChar, NULL);

      char* colorRChar = strtok(NULL, ",");
      if (colorRChar == NULL) exit(EXIT_FAILURE);
      double colorR = strtod(colorRChar, NULL);
      char* colorGChar = strtok(NULL, ",");
      if (colorGChar == NULL) exit(EXIT_FAILURE);
      double colorG = strtod(colorGChar, NULL);
      char* colorBChar = strtok(NULL, "\n");
      if (colorBChar == NULL) exit(EXIT_FAILURE);
      double colorB = strtod(colorBChar, NULL);

      ro = (RenderObject*)malloc(sizeof(RenderObject));
      createAndPackSphere(radius, createVector(centerX, centerY, centerZ), ro, (Color){colorR, colorG, colorB, 1});
      addToList(renderList, ro);
    } 
    else if (strcmp(type, "L") == 0) {
      char* positionXChar = strtok(NULL, ",");
      if (positionXChar == NULL) exit(EXIT_FAILURE);
      double positionX = strtod(positionXChar, NULL);
      char* positionYChar = strtok(NULL, ",");
      if (positionYChar == NULL) exit(EXIT_FAILURE);
      double positionY = strtod(positionYChar, NULL);
      char* positionZChar = strtok(NULL, ",");
      if (positionZChar == NULL) exit(EXIT_FAILURE);
      double positionZ = strtod(positionZChar, NULL);

      char* intenstyChar = strtok(NULL, ",");
      if (intenstyChar == NULL) exit(EXIT_FAILURE);
      double intensity = strtod(intenstyChar, NULL);

      char* constantCompChar = strtok(NULL, ",");
      if (constantCompChar == NULL) exit(EXIT_FAILURE);
      double constantComp = strtod(constantCompChar, NULL);
      char* linearCompChar = strtok(NULL, ",");
      if (linearCompChar == NULL) exit(EXIT_FAILURE);
      double linearComp = strtod(linearCompChar, NULL);
      char* cuadraticCompChar = strtok(NULL, "\n");
      if (cuadraticCompChar == NULL) exit(EXIT_FAILURE);
      double cuadraticComp = strtod(cuadraticCompChar, NULL);

      lights[i] = createLight((Vector){positionX, positionY, positionZ}, intensity, constantComp, linearComp, cuadraticComp);
      i++;
    }
  }


  fclose(fp);
  if (line)
    free(line);
  return lights;
}

int main (int argc, char** argv) {
  if (argc != 3){
    printf("uso: raytracing.bin <archivo_entrada> <archivo_salida>");
    exit(EXIT_FAILURE);
  } 
  renderList = (RenderList*)malloc(sizeof(RenderList));
  eye = (Vector*)malloc(sizeof(Vector));
  w = (Window*)malloc(sizeof(Window));

  // plane
  RenderObject* ro;
  Vector** points1 = (Vector**) malloc (sizeof(Vector*) * 5);

  points1[0] = createVector( 0.0 , 1.0 , 0.0 );
  points1[1] = createVector( -0.9510565400123596 , 0.30901697278022766 , -0.9510565400123596 );
  points1[2] = createVector( -0.5877851843833923 , -0.8090170621871948 , -0.5877851843833923 );
  points1[3] = createVector( 0.5877853631973267 , -0.8090169429779053 , 0.5877853631973267 );
  points1[4] = createVector( 0.9510564804077148 , 0.3090171217918396 , 0.9510564804077148 );

  ro = (RenderObject*)malloc(sizeof(RenderObject));
  createAndPackPlane(5, points1, ro, (Color){1, 0.5, 0.2, 1});
  addToList(renderList, ro);
  // end plane

  initializeFrameBuffer();
  initializeEye(eye);
  initializeWindow(w);
  lights = readFromFile(argv[1], renderList); 

  render(w, eye, lights, lightCount, frameBuffer, renderList);

  frameBufferToFile(argv[2], frameBuffer, H_SIZE, V_SIZE);

  return 0;
}

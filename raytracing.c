#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "vector.h"
#include "raytracing.h"
#include "renderEngine.h"
#include "Helpers/linkedlist.h"
#include "objects/cylinder.h"
#include "file.h"

Color** frameBuffer;
RenderList* renderList;

LinkedList *lights;
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
  readFromFile(argv[1], renderList, lights); 

//  Cylinder *c = (Cylinder*)malloc(sizeof(Cylinder));
//  RenderObject* ro = (RenderObject*)malloc(sizeof(RenderObject));
//  c->radius = 2;
//  c->center = createVector(0, 0, 0);
//  c->rotation = createVector(0, 0, 0);
//  ro->object = c;
//  ro->intersectionFunction = &rayCylinderIntersection;
//  ro->normalFunction = &cylinderNormal;
//  ro->diffuseCoefficient = 1;
//  ro->specularCoefficient = 32;
//  ro->type = SphereObject;
//  ro->color = (Color){0.8, 0.8, 0.8, 1};
//  addToList(renderList, ro);

  render(w, eye, lights, frameBuffer, renderList);

  frameBufferToFile(argv[2], frameBuffer, H_SIZE, V_SIZE);

  return 0;
}

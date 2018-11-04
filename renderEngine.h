#ifndef H_RENDER_ENGINE
#define H_RENDER_ENGINE
#include "Helpers/linkedlist.h"
#include "objects/objects.h"

void render (Window* w, Vector* eye, LinkedList* light, Color** frameBuffer, RenderList* renderList);

#define H_SIZE 1008
#define V_SIZE 576
#define bgColor (Color){0.7, 0.7, 0.7, 1}
#define bgIntensity 0.4
#define diffuseCorrection 0.8
#define EPSILON 0.001

#endif

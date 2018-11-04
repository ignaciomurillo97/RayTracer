#include "objects.h"
#include <stdlib.h>
#include <math.h>

void addToList (RenderList* list, RenderObject* obj) {
  if (list->start == NULL) {
    list->start = obj;
    list->end = obj;
    list->end->next = NULL;
  } else {
    list->end->next = obj;
    list->end = obj;
    list->end->next = NULL;
  }

}

Vector framebuffToUniverse (Window* w, int x, int y) {
  return (Vector){
    w->left + ((x + 0.5) / (float)w->pixelWidth) * (w->right - w->left),
      w->bot + ((y + 0.5) / (float)w->pixelHeight) * (w->top - w->bot)
  };
}

double attenuationFactor (Light light, double distToLight) {
  return light.cConstant + light.cLinear * distToLight + light.cCuadratic * pow(distToLight, 2);
}

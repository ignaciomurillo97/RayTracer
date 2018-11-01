#include "objects.h"
#include <stdlib.h>

void addToList (RenderList* list, RenderObject* obj) {
  if (list->start == NULL) {
    list->start = obj;
    list->end = obj;
  } else {
    list->end->next = obj;
    list->end = obj;
  }

}

Vector framebuffToUniverse (Window* w, int x, int y) {
  return (Vector){
    w->left + ((x + 0.5) / (float)w->pixelWidth) * (w->right - w->left),
      w->bot + ((y + 0.5) / (float)w->pixelHeight) * (w->top - w->bot)
  };
}

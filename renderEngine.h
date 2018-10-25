#ifndef H_RENDER_ENGINE
#define H_RENDER_ENGINE

void render (Window* w, Vector* eye, Light* light, int lightCount , Color** frameBuffer, RenderList* renderList);

#define H_SIZE 1008
#define V_SIZE 576
#define bgColor (Color){0.2, 0.2, 0.2, 1}
#define bgIntensity 0.05
#define diffuseCorrection 0.8

#endif

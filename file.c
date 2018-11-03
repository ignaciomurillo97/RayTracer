#include <stdlib.h>
#include "file.h"
#include "objects/sphere.h"
#include "objects/polygon.h"
#include "Helpers/wildCompare.h"


void frameBufferToFile(char* file, Color** frameBuffer, int hSize, int vSize){
  int i,j;
  FILE *fptr;

  if ((fptr = fopen(file,"w")) == NULL) {
    fprintf(stderr, "Error al abrir el archivo \"%s\"\n", file);
    exit(-1);
  }

  int fixedHSize = hSize, fixedVSize = vSize;
  FIX(fixedHSize);
  FIX(fixedVSize);

  fwrite(&fixedHSize, sizeof(int), 1, fptr);
  fwrite(&fixedVSize, sizeof(int), 1, fptr);

  for (j = 0; j < vSize; j++) {
    for (i = 0; i < hSize; i++) {
      writePixel(frameBuffer[i][j], fptr);
    }
  }

  fclose(fptr);
}

void writeColorComponent(double component, FILE* file){
  int componentInt = component * MAX_COLOR;
  fwrite(&componentInt, sizeof(unsigned char), 1, file);
}


void writePixel(Color pixelColor, FILE *file){
  writeColorComponent(pixelColor.a, file);
  writeColorComponent(pixelColor.r, file);
  writeColorComponent(pixelColor.g, file);
  writeColorComponent(pixelColor.b, file);
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

void createAndPackSphere(double radius, Vector* center, RenderObject* ro, Color color, double diffuseCoefficient, double specularCoefficient) {
  Sphere* s = (Sphere*)malloc(sizeof(Sphere));
  s->radius = radius;
  s->center = center;
  ro->object = s;
  ro->intersectionFunction = &raySphereIntersection;
  ro->normalFunction = &sphereNormal;
  ro->diffuseCoefficient = diffuseCoefficient;
  ro->specularCoefficient = specularCoefficient;
  ro->type = SphereObject;
  ro->color = color;
}

void createAndPackPlane(int pointCount, Vector** points, RenderObject* ro, Color color, double diffuseCoefficient, double specularCoefficient) {
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
  ro->diffuseCoefficient = diffuseCoefficient;
  ro->specularCoefficient = specularCoefficient;
  ro->type = PlaneObject;
  ro->color = color;
}

// TODO cambiar para usar scanf
void readFromFile(char* filename, RenderList* renderList, LinkedList* lights) {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  RenderObject*  ro;

  fp = fopen(filename, "r");
  if (fp == NULL){
    exit(EXIT_FAILURE);
  }

  while ((getline(&line, &len, fp)) != -1) {
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
        addToLinkedList((void*) createLight((Vector){positionX, positionY, positionZ}, intensity, constantComp, linearComp, cuadraticComp), lights);
      } else if (wildcmp("sphere*", line)) {
        double radius;
        double diffuseCoefficient, specularCoefficient;
        double centerX, centerY, centerZ;
        double colorR, colorG, colorB;

        char* format = "sphere %lf (%lf, %lf, %lf) (%lf, %lf, %lf) %lf %lf";
        int itemsRead = sscanf(line, format, &radius, &centerX, &centerY, &centerZ, &colorR, &colorG, &colorB, &diffuseCoefficient, &specularCoefficient);
        if (itemsRead != 9) exit (EXIT_FAILURE);
        ro = (RenderObject*)malloc(sizeof(RenderObject));
        createAndPackSphere(radius, createVector(centerX, centerY, centerZ), ro, (Color){colorR, colorG, colorB, 1}, diffuseCoefficient, specularCoefficient);
        addToList(renderList, ro);
      } else if (wildcmp("polygon*", line)) {
        int vertexCount;
        double diffuseCoefficient, specularCoefficient;
        Color color = (Color){};
        int i = 0;
        char* format = "polygon (%d) %lf %lf (%lf, %lf, %lf) {";
        if (sscanf(line, format, &vertexCount, &diffuseCoefficient, &specularCoefficient, &color.r, &color.g, &color.b) != 6) {
          exit (EXIT_FAILURE);
        } 
        Vector** vertices = (Vector**)malloc(sizeof(Vector*) * vertexCount);

        while ((getline(&line, &len, fp)) != -1) {
          if (wildcmp("*}*", line)) break;
          if (wildcmp("*vector *", line)) {
            Vector* v = (Vector*)malloc(sizeof(Vector));
            if (sscanf(line, "\tvector (%lf, %lf, %lf)", &v->x, &v->y, &v->z) != 3) exit(EXIT_FAILURE);
            vertices[i] = v;
            i++;
          }
        }

        ro = (RenderObject*)malloc(sizeof(RenderObject));
        createAndPackPlane(vertexCount, vertices, ro, (Color){color.r, color.g, color.b}, diffuseCoefficient, specularCoefficient);
        addToList(renderList, ro);
      }
    }
  }

  fclose(fp);
  if (line)
    free(line);
}

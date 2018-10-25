#include "file.h"


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

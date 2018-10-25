#ifndef H_FILE
#define H_FILE
#include "raytracing.h"
#include "stdio.h"
#include "stdlib.h"

#define SWAP(x) ( ((x) << 24) | \
         (((x) << 8) & 0x00ff0000) | \
         (((x) >> 8) & 0x0000ff00) | \
         ((x) >> 24) )

#define FIX(x) (*(unsigned *)&(x) = \
         SWAP(*(unsigned *)&(x)))


void writePixel(Color pixelColor, FILE *file);
void frameBufferToFile(char* file, Color** frameBuffer, int hSize, int vSize);
#endif

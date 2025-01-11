#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CT_SIZE 1024 // Bitmap color table size
#define HEADER_SIZE 54

typedef struct {
    unsigned char header[HEADER_SIZE];
    unsigned int width;
    unsigned int height;
    unsigned int bitDepth;
    bool CT_EXISTS;
    unsigned char colorTable[CT_SIZE];
    unsigned char *imageBuffer;
} bitmap;

void readImage(char *filename1, bitmap *bitmapIn) {
    printf("readImage: %d\n", bitmapIn->width);
    FILE *streamIn = fopen(filename1, "rb");
    if (streamIn == NULL) {
        printf("Error opening file!\n");
    } // TODO: need to return error from here.

    for (int i = 0; i < HEADER_SIZE; i++) {
        bitmapIn->header[i] = getc(streamIn);
    }
    // TODO: Make sure bitmap is initialized somewhere.
    bitmapIn->width = *(int *)&bitmapIn->header[18];
    bitmapIn->height = *(int *)&bitmapIn->header[22];
    bitmapIn->bitDepth = *(int *)&bitmapIn->header[28];

    const int IMAGE_SIZE = bitmapIn->width * bitmapIn->height;

    bitmapIn->CT_EXISTS = false; // should already be initialized somewhere.
    if (bitmapIn->bitDepth <=
        8) { // by definition of bitmap, <= 8 has a color table
        bitmapIn->CT_EXISTS = true;
    }

    if (bitmapIn->CT_EXISTS) {
        fread(bitmapIn->colorTable, sizeof(char), CT_SIZE, streamIn);
    }
    bitmapIn->imageBuffer = (char *)malloc(IMAGE_SIZE);
    // unsigned char buffer[IMAGE_SIZE];

    fread(bitmapIn->imageBuffer, sizeof(char), IMAGE_SIZE, streamIn);
    fclose(streamIn);
}

int main(int argc, char *argv[]) {

    char *filename1 = NULL;
    char *filename2 = NULL;

    if (argc > 1) {
        filename1 = argv[1];
        printf("Filename1: %s\n", filename1);
    }
    if (argc > 2) {
        filename2 = argv[2];
        printf("Filename2: %s\n", filename2);
    }
    bitmap bitmapIn;
    readImage(filename1, &bitmapIn);

    // width starts at address of byte(char) 18, which is then cast to an int*,
    // so it can be dereferenced into an int, so it is cast to a 4 byte int
    // instead stead of a single byte from the char header array. Then the
    // height can be retreived from the next 4 byts and so on.

    // if the bit depth is less than or equal to 8 then we need to read the
    // color table. The read content is going to be stored in colorTable. Not
    // all bitmap images have color tables.

    FILE *streamOut = fopen(filename2, "wb");
    fwrite(bitmapIn.header, sizeof(char), HEADER_SIZE, streamOut);

    if (bitmapIn.CT_EXISTS) {
        fwrite(bitmapIn.colorTable, sizeof(char), CT_SIZE, streamOut);
    }
    unsigned int imageSize = bitmapIn.width * bitmapIn.height;
    fwrite(bitmapIn.imageBuffer, sizeof(char), imageSize, streamOut);
    fclose(streamOut);

    printf("width: %d\n", bitmapIn.width);
    printf("height: %d\n", bitmapIn.height);
    printf("bitDepth: %d\n", bitmapIn.bitDepth);

    // free(bitmapIn.imageBuffer);
    return 0;
}
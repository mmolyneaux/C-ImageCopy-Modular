#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CT_SIZE 1024   // Bitmap color table size
#define HEADER_SIZE 54 // Bitmap file header size

typedef struct {
    unsigned char header[HEADER_SIZE];
    unsigned int width;
    unsigned int height;
    unsigned int bitDepth;
    bool CT_EXISTS;
    unsigned char colorTable[CT_SIZE];
    unsigned char *imageBufferPtr;
} bitmap;

bool readImage(char *filename1, bitmap *bitmapIn) {
    bool imageRead = false;
    printf("readImage: %d\n", bitmapIn->width);
    FILE *streamIn = fopen(filename1, "rb");
    if (streamIn == NULL) {
        printf("Error opening file!\n");
        return imageRead;
    }
    imageRead = true; // TODO: need to return error from here.

    for (int i = 0; i < HEADER_SIZE; i++) {
        bitmapIn->header[i] = getc(streamIn);
    }

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
    bitmapIn->imageBufferPtr = (char *)malloc(IMAGE_SIZE);
    // unsigned char buffer[IMAGE_SIZE];

    fread(bitmapIn->imageBufferPtr, sizeof(char), IMAGE_SIZE, streamIn);
    fclose(streamIn);
    return imageRead = true;
}

int main(int argc, char *argv[]) {

    char *filename1 = NULL;
    char *filename2 = NULL;
    const char *suffix = "_copy";
    const char *extension = ".bmp";

    if (argc > 1) {
        filename1 = argv[1];
        printf("Filename1: %s\n", filename1);
    }

    // If second filename exists from argv point to that.
    if (argc > 2) {
        filename2 = argv[2];

        // else create a filename based on the first and allocate memory for the
        // new name.
    } else {
        // Finds the last position of the  '.' in the filename
        char *dot_pos = strrchr(filename1, '.');
        if (dot_pos == NULL) {
            printf("\".\" not found in filename.\n");
            return -1;
        }
        
        // Calculate the length of the parts
        size_t base_len = dot_pos - filename1;
        size_t suffix_len = strlen(suffix);
        size_t extention_len = strlen(extension);
        printf("Filename1: %s", filename1);
        filename2 = (char *)calloc(base_len + suffix_len + extention_len + 1,
                                   sizeof(char));
        if (filename2 == NULL) {
            printf("Memory allocation for output filename has failed.\n");
            return -1;
        }
        // Copy the base part of filename1 and append the suffix and extension.
        // strncpy copies the first base_len number of chars from filename1 into
        // filename2
        strncpy(filename2, filename1, base_len);
        // use ptr math to copy suffix to filename2ptr's + position + (can't use
        // strcat because strncpy doesn't null terminate.)
        strcpy(filename2 + base_len, suffix);
        strcpy(filename2 + base_len + suffix_len, extension);

        printf("Filename2: %s\n", filename2);
    }

    bitmap bitmapIn = {.header = {0},
                       .width = 0,
                       .height = 0,
                       .bitDepth = 0,
                       .CT_EXISTS = false,
                       .colorTable = {0},
                       .imageBufferPtr = NULL};

    bool imageRead = readImage(filename1, &bitmapIn);

    // width starts at address of byte(char) 18, which is then cast to an int*,
    // so it can be dereferenced into an int, so it is cast to a 4 byte int
    // instead stead of a single byte from the char header array. Then the
    // height can be retreived from the next 4 byts and so on.

    // if the bit depth is less than or equal to 8 then we need to read the
    // color table. The read content is going to be stored in colorTable. Not
    // all bitmap images have color tables.

    FILE *streamOut = fopen(filename2, "wb");
    free(filename2);
    fwrite(bitmapIn.header, sizeof(char), HEADER_SIZE, streamOut);

    if (bitmapIn.CT_EXISTS) {
        fwrite(bitmapIn.colorTable, sizeof(char), CT_SIZE, streamOut);
    }
    unsigned int imageSize = bitmapIn.width * bitmapIn.height;
    fwrite(bitmapIn.imageBufferPtr, sizeof(char), imageSize, streamOut);
    fclose(streamOut);

    printf("width: %d\n", bitmapIn.width);
    printf("height: %d\n", bitmapIn.height);
    printf("bitDepth: %d\n", bitmapIn.bitDepth);

    // free(bitmapIn.imageBufferPtr);

    return 0;
}
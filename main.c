#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define CT_SIZE 1024 // Bitmap color table size
#define HEADER_SIZE 54

int main(int argc, char *argv[]) {

    FILE *streamIn = fopen("images/cameraman.bmp", "rb");
    FILE *streamOut = fopen("images/cameraman_copy.bmp", "wb");

    if (streamIn == NULL) {
        printf("Error opening file!\n");
    }

    unsigned char header[HEADER_SIZE];
    unsigned char colorTable[CT_SIZE];

    for (int i = 0; i < HEADER_SIZE; i++) {
        header[i] = getc(streamIn);
    }

    // width starts at address of byte(char) 18, which is then cast to an int*,
    // so it can be dereferenced into an int, so it is cast to a 4 byte int
    // instead stead of a single byte from the char header array. Then the
    // height can be retreived from the next 4 byts and so on.
    unsigned int width = *(int *)&header[18];
    unsigned int height = *(int *)&header[22];
    unsigned int bitDepth = *(int *)&header[28];

    const unsigned int IMAGE_SIZE = width * height;

    // if the bit depth is less than or equal to 8 then we need to read the
    // color table. The read content is going to be stored in colorTable. Not
    // all bitmap images have color tables.    
    bool CT_EXISTS = false;
    if (bitDepth <= 8) {
        CT_EXISTS = true;
    }

    if (CT_EXISTS) {
        fread(colorTable, sizeof(unsigned char), CT_SIZE, streamIn);
    }

    fwrite(header, sizeof(unsigned char), HEADER_SIZE, streamOut);

    unsigned char buffer[IMAGE_SIZE];

    fread(buffer, sizeof(unsigned char), IMAGE_SIZE, streamIn);

    if (CT_EXISTS) {
        fwrite(colorTable, sizeof(unsigned char), CT_SIZE, streamOut);
    }
    fwrite(buffer, sizeof(unsigned char), IMAGE_SIZE, streamOut);
    fclose(streamOut);
    fclose(streamIn);
    
    
    printf("width: %d\n", width);
    printf("height: %d\n", height);
    printf("bitDepth: %d\n", bitDepth);

    return 0;
}
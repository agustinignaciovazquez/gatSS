#ifndef BMP_BMP_H
#define BMP_BMP_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>

#define BMP_TYPE 0x4D42

#pragma pack(push,1)
typedef struct {             // Total: 54 bytes
    uint16_t  type;             // Magic identifier: 0x4d42
    uint32_t  size;             // File size in bytes
    uint16_t  reserved1;        // Not used
    uint16_t  reserved2;        // Not used
    uint32_t  offset;           // Offset to image data in bytes from beginning of file (54 bytes)
    uint32_t  dib_header_size;  // DIB Header size in bytes (40 bytes)
    int32_t   width_px;         // Width of the image
    int32_t   height_px;        // Height of image
    uint16_t  num_planes;       // Number of color planes
    uint16_t  bits_per_pixel;   // Bits per pixel
    uint32_t  compression;      // Compression type
    uint32_t  image_size_bytes; // Image size in bytes
    int32_t   x_resolution_ppm; // Pixels per meter
    int32_t   y_resolution_ppm; // Pixels per meter
    uint32_t  num_colors;       // Number of colors
    uint32_t  important_colors; // Important colors
} BMPHeader;
#pragma pack(pop)

typedef struct {
    FILE * fd;
    BMPHeader header;
    uint8_t * data;
    uint8_t * extra_header;
} BMPImage;

/* Read a BMP Image from the file descriptor fd */
BMPImage * read_bmp(FILE * fd);

/* Write a BMP Image into the file descriptor fd */
int write_bmp(BMPImage * image, FILE * fd);

/* Free a BMP Image */
void free_bmp(BMPImage * image);

/* Copy a BMP Image */
BMPImage * copy_bmp(BMPImage * src);

/* Checks if BMP Image has valid header */
bool bmp_valid_header(BMPHeader * header);

/* Creates a new BMP Image */
BMPImage * new_bmp_image();

BMPImage * create_bmp(const uint8_t* values, uint32_t h, uint32_t w, uint8_t * extraHeader);

/* Returns a list of with the files (strings) that are bmp in dp directory.
 * count: specifies how many bmps should we find in this directory.
 * found: specifies how many bmps we actually found in this directory.
 * */
char ** bmps_in_dir(DIR *dp, int count, int * found);

/* Frees a list of bmp file names */
void bmp_free_list(BMPImage ** bmp_list, int len);

/* A utility function used to sort the bmp files */
int compare_strings(const void *a, const void *b);

/* Opens all bmp files specified by file_list */
BMPImage ** open_files(char ** file_list, int to_open, char *dir);

/* Prints information about the bmp files opened */
void print_bmps_info(BMPImage ** bmp_list, char ** file_list, int len);

int check_bmp_sizes(BMPImage ** bmp_list, int len);

#endif // BMP_BMP_H


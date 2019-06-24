//
// Created by smanganaro
//

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
    uint32_t  bmp_header_size;  // DIB Header size in bytes (40 bytes)
    int32_t   width;         // Width of the image
    int32_t   height;        // Height of image
    uint16_t  num_planes;       // Number of color planes
    uint16_t  bits_per_pixel;   // Bits per pixel
    uint32_t  compression;      // Compression type
    uint32_t  image_size_bytes; // Image size in bytes
    int32_t   horizontal_res; // Pixels per meter
    int32_t   vertical_res; // Pixels per meter
    uint32_t  color_table_size;       // Number of colors
    uint32_t  count_important_colors; // Important colors
} BMPHeader;
#pragma pack(pop)

typedef struct {
    FILE * fd;
    BMPHeader header;
    uint8_t * data;
    uint8_t * header2;
} BMPImage;

/*Read a BMP Image from file descriptor */
BMPImage * bmp_read(FILE * fd);

/* Write a BMP Image into file descriptor */
int bmp_write(BMPImage * image, FILE * fd);

/* Creates a new BMP Image */
BMPImage * bmp_create_image();

/* Build a BMP Image*/
BMPImage * build_image(BMPImage * base);

/* Copies a BMP Image */
BMPImage * copy_bmp(BMPImage * src);

/* Frees a BMP Image */
void bmp_free(BMPImage * image);

/*Frees a list of bmp file names */
void bmp_free_list(BMPImage ** bmp_list, int len);

/* Used to order the bmp files */
BMPImage ** bpm_open_files(char ** file_list, int to_open, char *dir);

/* Used to order the bmp files */
char ** bmps_in_dir(DIR *dp, int count, int * found);

/*Prints information about the bmp files opened*/
void bmp_print_info(BMPImage ** bmp_list, char ** file_list, int len);

/* Compares the bmp filenames*/
int compare_strings(const void *a, const void *b);

/* Checks if bmp file sizes are the expected ones */
int check_bmp_sizes(BMPImage ** bmp_list, int len);

/* Checks if BMP Image header is valid */
bool bmp_valid_header(BMPHeader * header);

#endif // BMP_BMP_H


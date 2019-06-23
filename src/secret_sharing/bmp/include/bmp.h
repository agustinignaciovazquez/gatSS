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

/**
 * @brief Read a BMP Image from the file descriptor fd
 *
 * @param FILE * fd
 * @return BMPImage*
 */
BMPImage * read_bmp(FILE * fd);

/**
 * @brief Write a BMP Image into the file descriptor fd
 *
 * @param FILE * fd
 * @param BMPImage * image
 * @return int
 */
int write_bmp(BMPImage * image, FILE * fd);

/**
 * @brief Free a BMP Image
 *
 * @param BMPImage * image
 * @return void
 */
void free_bmp(BMPImage * image);

/**
 * @brief Copy a BMP Image
 *
 * @param BMPImage * src
 * @return BMPImage*
 */
BMPImage * copy_bmp(BMPImage * src);

/**
 * @brief Checks if BMP Image has valid header
 *
 * @param BMPHeader * header
 * @return bool
 */
bool bmp_valid_header(BMPHeader * header);

/**
 * @brief Creates a new BMP Image
 *
 * @return BMPImage*
 */
BMPImage * new_bmp_image();

/**
 * @brief Returns a list of with the files (strings) that are bmp in dp directory.
 *
 * @param int count: specifies how many bmps should we find in this directory.
 * @param int found: specifies how many bmps we actually found in this directory
 * @param DIR *dp
 * @return char**
 */
char ** bmps_in_dir(DIR *dp, int count, int * found);

/**
 * @brief Frees a list of bmp file names
 *
 * @param BMPImage** bmp_list
 * @param int len
 * @return void
 */
void bmp_free_list(BMPImage ** bmp_list, int len);

/**
 * @brief A utility function used to sort the bmp files
 *
 * @param const void *a
 * @param const void *b
 * @return void
 */
int compare_strings(const void *a, const void *b);

/**
 * @brief A utility function used to sort the bmp files
 *
 * @param char ** file_list
 * @param int to_open
 * @param char *dir
 * @return BMPImage **
 */
BMPImage ** open_files(char ** file_list, int to_open, char *dir);

/**
* @brief Prints information about the bmp files opened
*
* @param BMPImage ** bmp_list
* @param char ** file_list
* @param int len
* @return void
*/
void print_bmps_info(BMPImage ** bmp_list, char ** file_list, int len);

/**
* @brief Check if the size of bmp are the correct ones
*
* @param BMPImage ** bmp_list
* @param int len
* @return int
*/
int check_bmp_sizes(BMPImage ** bmp_list, int len);

/**
* @brief Build an image
*
* @param BMPImage * base
* @return BMPImage *
*/
BMPImage * build_image(BMPImage * base);

#endif // BMP_BMP_H


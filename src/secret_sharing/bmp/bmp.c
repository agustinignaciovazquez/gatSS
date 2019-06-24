#include "include/bmp.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR -1
#define OK 1

#define MAX_FILENAME_LEN 255

BMPImage * bmp_read(FILE * fd) {

    BMPImage * image = bmp_create_image();

    size_t read = 0;

    if(image == NULL) {
        return NULL;
    }
    image->fd = fd;

    read = fread(&image->header, sizeof(image->header), 1, image->fd); //Leer header

    if(read != 1) {
        fclose(fd);
        return NULL;
    }

    if(!bmp_valid_header(&image->header)) {
        return NULL;
    }

    if(fseek(image->fd, image->header.offset, 0) != 0) { //Ir al pixel array
        return NULL;
    }
    image->data = malloc(image->header.image_size_bytes);

    read = fread(image->data, sizeof(uint8_t), image->header.image_size_bytes, image->fd); //Leer los pixeles

    if(read != image->header.image_size_bytes) {
        return NULL;
    }

    size_t header2_size = image->header.offset - sizeof(image->header);

    if(fseek(image->fd, sizeof(image->header), 0) != 0) {
        return NULL;
    }
    image->header2 = malloc(header2_size);

    read = fread(image->header2, sizeof(uint8_t), header2_size, image->fd);

    if(read != header2_size) {
        return NULL;
    }

    return image;

}

BMPImage * bmp_create_image() {
    BMPImage * image = (BMPImage *) malloc(sizeof(BMPImage));
    return image;
}

bool bmp_valid_header(BMPHeader * header) {
    if(header->type != BMP_TYPE) {
        return false;
    }
    return true;

}

void bmp_free(BMPImage * image) {
    free(image);
}

/* Returns the size of the extra header */
int get_header2_size(BMPImage * image) {
    return image->header.offset - sizeof(image->header);
}



/* Copies an image */

BMPImage * copy_bmp(BMPImage * src) {
    BMPImage * new_image = bmp_create_image();
    memcpy(&new_image->header, &src->header, sizeof(src->header));
    new_image->data = malloc(src->header.image_size_bytes);
    memcpy(new_image->data, src->data, src->header.image_size_bytes);

    int header2_size = get_header2_size(src);
    new_image->header2 = malloc(header2_size);
    memcpy(new_image->header2, src->header2, header2_size);
    new_image->fd = src->fd;
    return new_image;
}

/* Writes a BMP image into fd */

int bmp_write(BMPImage * image, FILE * fd) {
    int written = fwrite(&image->header, sizeof(image->header),1, fd);
    if(written != 1) {
        return ERROR;
    }
    int header2_size = get_header2_size(image);
    written = fwrite(image->header2, sizeof(*image->header2),header2_size, fd);
    if(written != header2_size) {
        return ERROR;
    }
    written = fwrite(image->data, sizeof(*image->data), image->header.image_size_bytes, fd);
    if(written != image->header.image_size_bytes) {
        return ERROR;
    }
    return OK;
}

uint32_t is_bmp_file(struct dirent *ep)
{
    char *filename = ep->d_name;
    size_t len = strlen(filename);
    if (len < 5 || ep->d_type != DT_REG)
    {
        return 0;
    }

    char *extension = &filename[len - 4];
    if (strcmp(extension, ".bmp") != 0)
    {
        return 0;
    }

    return 1;
}

char ** bmps_in_dir(DIR *dp, int count, int *found) {
    size_t bmp_count = 0;
    struct dirent *ep;

    if (dp == NULL || found == NULL) {
        return NULL;
    }

    while ((ep = readdir(dp))) {
        if (is_bmp_file(ep)) {
            bmp_count++;
        }
    }

    if (bmp_count == 0 || (count != 0 && bmp_count < count)) {
        *found = bmp_count;
        return NULL;
    }

    rewinddir(dp);

    char ** bmps = malloc(bmp_count * sizeof(char*));
    if (bmps == NULL) {
        return NULL;
    }

    uint32_t i = 0;
    while ((ep = readdir(dp))) {
        if (is_bmp_file(ep)) {
            bmps[i++] = ep->d_name;
        }
    }

    qsort(bmps, bmp_count, sizeof(char*), compare_strings);
    *found = bmp_count;

    return bmps;
}

BMPImage ** bmp_open_files(char ** file_list, int to_open, char *dir) {
    uint32_t i;
    BMPImage ** bmp_list = malloc(to_open * sizeof(struct bmp_handle*));
    char tmp_filename[MAX_FILENAME_LEN] = {0};

    if (bmp_list == NULL) {
        return NULL;
    }

    for (i = 0; i < to_open; i++)
    {
        strcpy(tmp_filename, dir);
        strcat(tmp_filename, "/");
        strcat(tmp_filename, file_list[i]);
        FILE * fd = fopen(tmp_filename, "r");
        bmp_list[i] = bmp_read(fd);
        if (bmp_list[i] == NULL) {
            bmp_free_list(bmp_list, i);
            return NULL;
        }
    }

    return bmp_list;
}

void bmp_free_list(BMPImage ** bmp_list, int len) {
    size_t i;
    for (i = 0; i < len; i++)
    {
        bmp_free(bmp_list[i]);
    }
}


int compare_strings(const void *a, const void *b) {
    return strcmp(*(char**)a, *(char**)b);
}

void bmp_print_info(BMPImage ** bmp_list, char ** file_list, int len) {
    uint32_t i;
    printf("Loaded images:\n");
    for (i = 0; i < len; i++) {
        BMPHeader * header = &bmp_list[i]->header;
        printf("%s [%ux%u] (Data Offset: 0x%x)\n", file_list[i], header->width, header->height, header->offset);
    }
}

int bmp_check_size(BMPImage ** bmp_list, int len) {
    if (len < 2)
    {
        return -1;
    }

    BMPHeader header = bmp_list[0]->header;
    uint32_t first_width = header.width;
    uint32_t first_height = header.height;

    uint32_t i;
    for (i = 1; i < len; i++)
    {
        header = bmp_list[i]->header;

        if (header.width != first_width || header.height != first_height)
        {
            return -1;
        }
    }

    return 0;
}

BMPImage * build_image(BMPImage * base) {
    BMPImage * new_image = copy_bmp(base);
    new_image->header2 = calloc(256 * 4, sizeof(uint8_t)); //Reservar espacio para la paleta de colores
    new_image->header.bits_per_pixel = 8; // 8 bits por pixel
    new_image->header.size = base->header.width * base->header.height + 1024;
    // El tamaño completo de la imagen es width * heigth + el espacio de la paleta
    new_image->header.offset = 1078; //Es 54 + 1024
    new_image->header.color_table_size = 256; // 256 colores
    new_image->header.image_size_bytes = base->header.width * base->header.height;

    /* Generacion de la paleta de colores */
    uint32_t jj=3;
    for(uint32_t ii=0;ii<255;ii++){
        new_image->header2[jj+1]=(uint8_t)ii+1;
        new_image->header2[jj+2]=(uint8_t)ii+1;
        new_image->header2[jj+3]=(uint8_t)ii+1;
        jj=jj+4;
    }

    return new_image;
}


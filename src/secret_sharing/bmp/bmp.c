#include "bmp.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR -1
#define OK 1

#define FILE_MAX_LEN 512

BMPImage * bmp_read(FILE * fd) {

    BMPImage * img = bmp_create_image();

    size_t read = 0;

    if(img == NULL) {
        return NULL;
    }
    img->fd = fd;

    read = fread(&img->header, sizeof(img->header), 1, img->fd); //Leer header

    if(read != 1) {
        fclose(fd);
        return NULL;
    }

    if(!bmp_valid_header(&img->header)) {
        return NULL;
    }

    if(fseek(img->fd, img->header.offset, 0) != 0) { //Ir al pixel array
        return NULL;
    }
    img->data = malloc(img->header.image_size_bytes);

    read = fread(img->data, sizeof(uint8_t), img->header.image_size_bytes, img->fd); //Leer los pixeles

    if(read != img->header.image_size_bytes) {
        return NULL;
    }

    size_t header2_size = img->header.offset - sizeof(img->header);

    if(fseek(img->fd, sizeof(img->header), 0) != 0) {
        return NULL;
    }
    img->header2 = malloc(header2_size);

    read = fread(img->header2, sizeof(uint8_t), header2_size, img->fd);

    if(read != header2_size) {
        return NULL;
    }

    return img;

}

BMPImage * bmp_create_image() {
    BMPImage * img = (BMPImage *) malloc(sizeof(BMPImage));
    return img;
}

bool bmp_valid_header(BMPHeader * header) {
    if(header->type != BMP_TYPE) {
        return false;
    }
    return true;

}

void bmp_free(BMPImage * img) {
    free(img);
}


int get_header2_size(BMPImage * img) {
    return img->header.offset - sizeof(img->header);
}

BMPImage * copy_bmp(BMPImage * src) {
    BMPImage * img_new = bmp_create_image();
    memcpy(&img_new->header, &src->header, sizeof(src->header));
    img_new->data = malloc(src->header.image_size_bytes);
    memcpy(img_new->data, src->data, src->header.image_size_bytes);

    int header2_size = get_header2_size(src);
    img_new->header2 = malloc(header2_size);
    memcpy(img_new->header2, src->header2, header2_size);
    img_new->fd = src->fd;
    return img_new;
}

int bmp_write(BMPImage * img, FILE * fd) {
    int written = fwrite(&img->header, sizeof(img->header),1, fd);
    if(written != 1) {
        return ERROR;
    }
    int header2_size = get_header2_size(img);
    written = fwrite(img->header2, sizeof(*img->header2),header2_size, fd);
    if(written != header2_size) {
        return ERROR;
    }
    written = fwrite(img->data, sizeof(*img->data), img->header.image_size_bytes, fd);
    if(written != img->header.image_size_bytes) {
        return ERROR;
    }
    return OK;
}

uint32_t is_bmp_file(struct dirent *ep)
{
    char *filename = ep->d_name;
    size_t len = strlen(filename);
    if (ep->d_type != DT_REG || len < 5 )
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
    size_t counter = 0;
    struct dirent *ep;

    if ( found == NULL || dp == NULL) {
        return NULL;
    }

    while ((ep = readdir(dp))) {
        if (is_bmp_file(ep)) {
            counter++;
        }
    }

    if ((count != 0 && counter < count) || counter == 0 ) {
        *found = counter;
        return NULL;
    }

    rewinddir(dp);

    char ** bmps = malloc(counter * sizeof(char*));
    if (bmps == NULL) {
        return NULL;
    }

    uint32_t i = 0;
    while ((ep = readdir(dp))) {
        if (is_bmp_file(ep)) {
            bmps[i++] = ep->d_name;
        }
    }

    qsort(bmps, counter, sizeof(char*), compare_strings);
    *found = counter;

    return bmps;
}

BMPImage ** bmp_open_files(char ** list_of_files, int to_open, char *dir) {
    uint32_t i;
    BMPImage ** list_of_bmp = malloc(to_open * sizeof(struct bmp_handle*));
    char tmp_filename[FILE_MAX_LEN] = {0};

    if (list_of_bmp == NULL) {
        return NULL;
    }

    for (i = 0; i < to_open; i++)
    {
        strcpy(tmp_filename, dir);
        strcat(tmp_filename, "/");
        strcat(tmp_filename, list_of_files[i]);
        FILE * fd = fopen(tmp_filename, "r");
        list_of_bmp[i] = bmp_read(fd);
        if (list_of_bmp[i] == NULL) {
            bmp_free_list(list_of_bmp, i);
            return NULL;
        }
    }

    return list_of_bmp;
}

void bmp_free_list(BMPImage ** list_of_bmp, int len) {
    size_t i;
    for (i = 0; i < len; i++)
    {
        bmp_free(list_of_bmp[i]);
    }
}


int compare_strings(const void *a, const void *b) {
    return strcmp(*(char**)a, *(char**)b);
}

int bmp_check_size(BMPImage ** list_of_bmp, int len) {
    if (len < 2)
    {
        return -1;
    }

    BMPHeader header = list_of_bmp[0]->header;
    uint32_t first_width = header.width;
    uint32_t first_height = header.height;

    uint32_t i;
    for (i = 1; i < len; i++)
    {
        header = list_of_bmp[i]->header;

        if (header.width != first_width || header.height != first_height)
        {
            return -1;
        }
    }

    return 0;
}

BMPImage * build_image(BMPImage * base) {
    BMPImage * img_new = copy_bmp(base);
    img_new->header2 = calloc(256 * 4, sizeof(uint8_t));
    img_new->header.bits_per_pixel = 8;
    img_new->header.size = base->header.width * base->header.height + 1024;

    img_new->header.offset = 1078;
    img_new->header.color_table_size = 256;
    img_new->header.image_size_bytes = base->header.width * base->header.height;

    uint32_t jj=3;
    for(uint32_t ii=0;ii<255;ii++){
        img_new->header2[jj+1]=(uint8_t)ii+1;
        img_new->header2[jj+2]=(uint8_t)ii+1;
        img_new->header2[jj+3]=(uint8_t)ii+1;
        jj=jj+4;
    }

    return img_new;
}


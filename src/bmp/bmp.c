#include <bmp/bmp.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mod_matrix/mod_matrix.h>
#include <logger/logger.h>

#define ERROR -1
#define OK 1

#define FILE_MAX_LEN 512

BMPImage * read_bmp(FILE * fd) {

    BMPImage * image = new_bmp_image();

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

    size_t extra_header_size = image->header.offset - sizeof(image->header);

    if(fseek(image->fd, sizeof(image->header), 0) != 0) {
        return NULL;
    }
    image->extra_header = malloc(extra_header_size);

    read = fread(image->extra_header, sizeof(uint8_t), extra_header_size, image->fd);

    if(read != extra_header_size) {
        return NULL;
    }

    return image;

}

BMPImage * new_bmp_image() {
    BMPImage * image = (BMPImage *) malloc(sizeof(BMPImage));
    return image;
}

bool bmp_valid_header(BMPHeader * header) {
    if(header->type != BMP_TYPE) {
        return false;
    }
    return true;

}

void free_bmp(BMPImage * image) {
    free(image);
}

/* Returns the size of the extra header */
int get_extra_header_size(BMPImage * image) {
    return image->header.offset - sizeof(image->header);
}



/* Copies an image */

BMPImage * copy_bmp(BMPImage * src) {
    BMPImage * new_image = new_bmp_image();
    memcpy(&new_image->header, &src->header, sizeof(src->header));
    new_image->data = malloc(src->header.image_size_bytes);
    memcpy(new_image->data, src->data, src->header.image_size_bytes);

    int extra_header_size = get_extra_header_size(src);
    new_image->extra_header = malloc(extra_header_size);
    memcpy(new_image->extra_header, src->extra_header, extra_header_size);
    new_image->fd = src->fd;
    return new_image;
}

/* Writes a BMP image into fd */

int write_bmp(BMPImage * image, FILE * fd) {
    int written = fwrite(&image->header, sizeof(image->header),1, fd);
    if(written != 1) {
        return ERROR;
    }
    int extra_header_size = get_extra_header_size(image);
    written = fwrite(image->extra_header, sizeof(*image->extra_header),extra_header_size, fd);
    if(written != extra_header_size) {
        return ERROR;
    }
    written = fwrite(image->data, sizeof(*image->data), image->header.image_size_bytes, fd);
    if(written != image->header.image_size_bytes) {
        return ERROR;
    }
    return OK;
}


BMPImage *
create_bmp(const uint8_t * values, uint32_t h, uint32_t w, uint8_t * extraHeader)
{
    BMPImage * image = new_bmp_image();

    image->header.type =  0x4d42;
    image->header.size = h*w + 1078;
    image->header.reserved1 = 0;
    image->header.reserved2 = 0;
    image->header.offset = 54;
    image->header.dib_header_size = 40;
    image->header.width_px = w;
    image->header.height_px = h;
    image->header.num_planes = 1;
    image->header.bits_per_pixel = 8;
    image->header.compression = 0;
    image->header.image_size_bytes = w*h;
    image->header.x_resolution_ppm = 0;
    image->header.y_resolution_ppm = 0;
    image->header.num_colors = 256;
    image->header.important_colors = 0;

    image->extra_header = calloc(1078, 1);
    image->data = malloc(w*h);
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            image->data[i*w + j] = (uint8_t) values[i*w + j];
        }
    }

    return image;

}

int is_bmp_file(struct dirent *ep)
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

    int i = 0;
    while ((ep = readdir(dp))) {
        if (is_bmp_file(ep)) {
            bmps[i++] = ep->d_name;
        }
    }

    qsort(bmps, bmp_count, sizeof(char*), compare_strings);
    *found = bmp_count;

    return bmps;
}

BMPImage ** open_files(char ** file_list, int to_open, char *dir) {
    int i;
    BMPImage ** bmp_list = malloc(to_open * sizeof(struct bmp_handle*));
    char tmp_filename[FILE_MAX_LEN] = {0};

    if (bmp_list == NULL) {
        return NULL;
    }

    for (i = 0; i < to_open; i++)
    {
        strcpy(tmp_filename, dir);
        strcat(tmp_filename, "/");
        strcat(tmp_filename, file_list[i]);
        FILE * fd = fopen(tmp_filename, "r");
        bmp_list[i] = read_bmp(fd);
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
        free_bmp(bmp_list[i]);
    }
}


int compare_strings(const void *a, const void *b) {
    return strcmp(*(char**)a, *(char**)b);
}

void print_bmps_info(BMPImage ** bmp_list, char ** file_list, int len) {
    int i;
    LOG_INFO("Loaded images:\n");
    for (i = 0; i < len; i++) {
        BMPHeader * header = &bmp_list[i]->header;
        LOG_INFO("%s [%ux%u] (Data Offset: 0x%x)\n", file_list[i], header->width_px, header->height_px, header->offset);
    }
}

int check_bmp_sizes(BMPImage ** bmp_list, int len) {
    if (len < 2)
    {
        return -1;
    }

    BMPHeader header = bmp_list[0]->header;
    uint32_t first_width = header.width_px;
    uint32_t first_height = header.height_px;

    int i;
    for (i = 1; i < len; i++)
    {
        header = bmp_list[i]->header;

        if (header.width_px != first_width || header.height_px != first_height)
        {
            return -1;
        }
    }

    return 0;
}

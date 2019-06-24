//
// Created by Agustin Vazquez on 2019-06-23.
//

#include "wrapper.h"
#include "distribute.h"
#include "recover.h"
#include "parser.h"

static inline uint32_t check_shadows(BMPImage *sec, BMPImage **shadows, size_t l, uint32_t n, uint32_t k) {
    BMPHeader secret_header = sec->header;
    uint32_t real_byte_count = secret_header.width * secret_header.height;
    size_t shadow_size = real_byte_count * (1.0 / k + 1.0 / n) * ((k == 4)? 8:4);

    for (uint32_t i = 0; i < l; i++) {
        BMPHeader header = shadows[i]->header;
        size_t byte_count = header.image_size_bytes;
        if (byte_count != shadow_size) {
            return -1;
        }
    }

    return 0;
}

int distribute_mode(Options options) {

    DIR * dp = opendir(options.directory);

    if(dp == NULL) {
        printf("Open directory failed.\n");
        return EXIT_FAILURE;
    }

    char ** file_list = NULL;
    int found = 0, to_open = 0;

    file_list = bmps_in_dir(dp, options.total_amount_of_shadows, &found);

    if(file_list == NULL) {
        printf("Open files in directory failed.\n");
        return EXIT_FAILURE;
    }

    to_open = options.total_amount_of_shadows;
    BMPImage ** bmp_list = bmp_open_files(file_list, to_open, options.directory);

    if (bmp_list == NULL) {
        printf("Open required files failed.\n");
        free(file_list);
        return EXIT_FAILURE;
    }

    if (bmp_check_size(bmp_list, to_open)) {
        printf("Images must be equal in width and height.\n");
        bmp_free_list(bmp_list, to_open);
        return EXIT_FAILURE;
    }

    FILE * watermark_fd = fopen(options.water_mark_file_name, "rb");

    if(watermark_fd == NULL) {
        printf("Open watermark image failed.\n");
        return EXIT_FAILURE;
    }

    BMPImage * watermark = bmp_read(watermark_fd);

    FILE * secret_fd = fopen(options.secret_file_name, "rb");

    if(secret_fd == NULL) {
        printf("Open secret image failed.\n");
        return EXIT_FAILURE;
    }

    if(watermark == NULL) {
        printf("Read watermark image failed.\n");
        return EXIT_FAILURE;
    }

    BMPImage * secret = bmp_read(secret_fd);

    if(secret == NULL) {
        printf("Read secret image failed.\n");
        return EXIT_FAILURE;

    }

    if(watermark->header.image_size_bytes != secret->header.image_size_bytes) {
        printf("Watermark image's and secret image's size must be equal.\n");
        return EXIT_FAILURE;
    }


    if (check_shadows(secret, bmp_list, to_open, options.total_amount_of_shadows, options.min_shadows_amount)) {
        printf("At least one shadow images does not have the required size.\n");
        bmp_free(secret);
        free(bmp_list);
        return EXIT_FAILURE;
    }

    distribute( secret, bmp_list, watermark, options.min_shadows_amount, options.total_amount_of_shadows);

    return EXIT_SUCCESS;

}

int recovery_mode(Options options){
    DIR * dp = opendir(options.directory);

    if(dp == NULL) {
        printf("Open directory failed.\n");
        return EXIT_FAILURE;
    }

    char ** file_list = NULL;
    int found = 0, to_open = 0;

    file_list = bmps_in_dir(dp, options.min_shadows_amount, &found);

    if(file_list == NULL) {
        printf("Open required files failed.\n");
        return EXIT_FAILURE;
    }

    to_open = options.min_shadows_amount;

    BMPImage ** bmp_list = bmp_open_files(file_list, to_open, options.directory);

    if (bmp_list == NULL) {
        printf("Open required files failed.\n");
        free(file_list);
        return EXIT_FAILURE;
    }

    if (bmp_check_size(bmp_list, to_open)) {
        printf("Images must be equal in width and height.\n");
        bmp_free_list(bmp_list, to_open);
        return EXIT_FAILURE;
    }

    FILE * rw_fd = fopen(options.water_mark_file_name, "rb");

    if(rw_fd == NULL) {
        printf("Open watermark image failed.\n");
        return EXIT_FAILURE;
    }

    BMPImage * rw_bmp = bmp_read(rw_fd);

    if(rw_bmp == NULL) {
        printf("Read watermark image failed.\n");
        return EXIT_FAILURE;
    }

    recover( options.secret_file_name, bmp_list, rw_bmp, options.min_shadows_amount, options.total_amount_of_shadows);

    return EXIT_SUCCESS;
}

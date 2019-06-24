//
// Created by Agustin Vazquez on 2019-06-23.
//

#include "wrapper.h"
#include "distribute.h"
#include "recover.h"
#include "parser.h"

static inline size_t shadow_size_for(uint32_t image_size, uint32_t n, uint32_t k) {
    uint32_t lsb_bytes;
    if (k == 4) {
        lsb_bytes = 8;
    } else {
        lsb_bytes = 4;
    }
    double factor = (1.0 / k + 1.0 / n);

    return image_size * factor * lsb_bytes;
}

static inline uint32_t check_shadow_sizes(BMPImage *secret, BMPImage **shadows, size_t len, uint32_t n, uint32_t k) {
    BMPHeader secret_header = secret->header;
    uint32_t real_byte_count = secret_header.width * secret_header.height;
    size_t shadow_size = shadow_size_for(real_byte_count, n, k);

    for (uint32_t i = 0; i < len; i++) {
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
        printf("Unable to open directory\n");
        return EXIT_FAILURE;
    }

    char ** file_list = NULL;
    int found = 0, to_open = 0;

    /* Count should be n, we need n image shares */
    file_list = bmps_in_dir(dp, options.total_amount_of_shadows, &found);
    /* found contains how many we actually found */
    if(file_list == NULL) {
        printf("Unable to open the files inside the directory\n");
        return EXIT_FAILURE;
    }


    if (found > options.total_amount_of_shadows) {
        printf("more than N = %d files were found, using N first files.\n", options.total_amount_of_shadows);

    }

    /* Open n images */
    to_open = options.total_amount_of_shadows;
    BMPImage ** bmp_list = bmp_open_files(file_list, to_open, options.directory);

    if (bmp_list == NULL) {
        printf("Error: Unable to open the required files (bmp_open_files).\n");
        free(file_list);
        return EXIT_FAILURE;
    }

    if (bmp_check_size(bmp_list, to_open)) {
        printf("Error: all images must have the same width and height.\n");
        bmp_free_list(bmp_list, to_open);
        return EXIT_FAILURE;
    }

    FILE * watermark_fd = fopen(options.water_mark_file_name, "rb");

    if(watermark_fd == NULL) {
        printf("Unable to open watermark\n");
        return EXIT_FAILURE;
    }

    BMPImage * watermark = bmp_read(watermark_fd);

    FILE * secret_fd = fopen(options.secret_file_name, "rb");

    if(secret_fd == NULL) {
        printf("Unable to open secret image\n");
        return EXIT_FAILURE;
    }

    if(watermark == NULL) {
        printf("Error reading watermark image\n");
        return EXIT_FAILURE;
    }

    BMPImage * secret = bmp_read(secret_fd);

    if(secret == NULL) {
        printf("Error reading secret image\n");
        return EXIT_FAILURE;

    }

    if(watermark->header.image_size_bytes != secret->header.image_size_bytes) {
        printf("Watermark image must have same size as secret image\n");
        return EXIT_FAILURE;
    }


    if (check_shadow_sizes(secret, bmp_list, to_open, options.total_amount_of_shadows, options.min_shadows_amount)) {
        printf("Error: one or more of the shadow images does not have the required size.\n");
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
        printf("Unable to open directory\n");
        return EXIT_FAILURE;
    }

    char ** file_list = NULL;
    int found = 0, to_open = 0;

    /* Count should be n, we need n image shares */
    file_list = bmps_in_dir(dp, options.min_shadows_amount, &found);
    /* found contains how many we actually found */

    if(file_list == NULL) {
        printf("Unable to open the files inside the directory\n");
        return EXIT_FAILURE;
    }


    if (found > options.min_shadows_amount) {
        printf("more than K = %d files were found, using K first files.\n", options.min_shadows_amount);

    }

    /* Open n images */
    to_open = options.min_shadows_amount;

    BMPImage ** bmp_list = bmp_open_files(file_list, to_open, options.directory);

    if (bmp_list == NULL) {
        printf("Error: Unable to open the required files (bmp_open_files).\n");
        free(file_list);
        return EXIT_FAILURE;
    }

    if (bmp_check_size(bmp_list, to_open)) {
        printf("Error: all images must have the same width and height.\n");
        bmp_free_list(bmp_list, to_open);
        return EXIT_FAILURE;
    }

    FILE * rw_fd = fopen(options.water_mark_file_name, "rb");

    if(rw_fd == NULL) {
        printf("Unable to open watermark\n");
        return EXIT_FAILURE;
    }

    BMPImage * rw_bmp = bmp_read(rw_fd);

    if(rw_bmp == NULL) {
        printf("Error reading watermark image\n");
        return EXIT_FAILURE;
    }

    recover( options.secret_file_name, bmp_list, rw_bmp, options.min_shadows_amount, options.total_amount_of_shadows);

    return EXIT_SUCCESS;
}

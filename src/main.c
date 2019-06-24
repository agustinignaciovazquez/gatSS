#include "distribute.h"
#include "recover.h"
#include "parser.h"
#include "utils.h"
#include "bmp.h"
#include <time.h>

#include <stdlib.h>

int distribute_mode(cmd_options options) {

    DIR * dp = opendir(options.dir);

    if(dp == NULL) {
        printf("Unable to open directory\n");
        return EXIT_FAILURE;
    }

    char ** file_list = NULL;
    int found = 0, to_open = 0;

    /* Count should be n, we need n image shares */
    file_list = bmps_in_dir(dp, options.n, &found);
    /* found contains how many we actually found */
    if(file_list == NULL) {
        printf("Unable to open the files inside the directory\n");
        return EXIT_FAILURE;
    }

    if (found > options.n) {
        printf("more than N = %d files were found, using N first files.\n", options.n);
    }

    /* Open n images */
    to_open = options.n;
    BMPImage ** bmp_list = bmp_open_files(file_list, to_open, options.dir);

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

    bmp_print_info(bmp_list, file_list, to_open);

    FILE * watermark_fd = fopen(options.watermark, "rb");

    if(watermark_fd == NULL) {
        printf("Unable to open watermark\n");
        return EXIT_FAILURE;
    }

    BMPImage * watermark = bmp_read(watermark_fd);

    FILE * secret_fd = fopen(options.secret, "rb");

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

    if (check_shadow_sizes(secret, bmp_list, to_open, options.n, options.k)) {
        printf("Error: one or more of the shadow images does not have the required size.\n");
        bmp_free(secret);
        free(bmp_list);
        return EXIT_FAILURE;
    }

    distribute(secret, bmp_list, watermark,options.k, options.n);

    return EXIT_SUCCESS;

}

int recovery_mode(cmd_options options){
    DIR * dp = opendir(options.dir);

    if(dp == NULL) {
        printf("Unable to open directory\n");
        return EXIT_FAILURE;
    }

    char ** file_list = NULL;
    int found = 0, to_open = 0;

    /* Count should be n, we need n image shares */
    file_list = bmps_in_dir(dp, options.k, &found);
    /* found contains how many we actually found */

    if(file_list == NULL) {
        printf("Unable to open the files inside the directory\n");
        return EXIT_FAILURE;
    }


    if (found > options.k) {
        printf("more than K = %d files were found, using K first files.\n", options.k);
    }

    /* Open n images */
    to_open = options.k;

    BMPImage ** bmp_list = bmp_open_files(file_list, to_open, options.dir);

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

    bmp_print_info(bmp_list, file_list, to_open);

    FILE * rw_fd = fopen(options.watermark, "rb");

    if(rw_fd == NULL) {
        printf("Unable to open watermark\n");
        return EXIT_FAILURE;
    }

    BMPImage * rw_bmp = bmp_read(rw_fd);

    if(rw_bmp == NULL) {
        printf("Error reading watermark image\n");
        return EXIT_FAILURE;
    }

    recover(options.secret, bmp_list, rw_bmp,options.k, options.n);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {

    rand_set_seed(1);

    cmd_options options;
    cmd_status status = parse_args(argc, argv, &options);

    if(status != CMD_SUCCESS) {
        print_error(status);
        return EXIT_FAILURE;
    }
    if (validate_args(&options)){
        return EXIT_FAILURE;
    }

    print_args_info(&options);

    if(options.mode == DISTRIBUTE_MODE){
        return distribute_mode(options);
    } else if (options.mode == RECOVER_MODE){
        return recovery_mode(options);
    }

    return EXIT_FAILURE;

}








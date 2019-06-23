#include <secret_sharing/distribute.h>
#include <secret_sharing/recover.h>
#include <secret_sharing/utils.h>
#include <args/args.h>
#include <algorithms/algorithms.h>
#include <time.h>
#include <bmp/bmp.h>
#include <mod/mod.h>
#include <rand/rand.h>
#include <stdlib.h>
#include <logger/logger.h>

int distribute_mode(cmd_options options) {

    DIR * dp = opendir(options.dir);

    if(dp == NULL) {
        LOG_ERROR("Unable to open directory\n");
        return EXIT_FAILURE;
    }

    char ** file_list = NULL;
    int found = 0, to_open = 0;

    /* Count should be n, we need n image shares */
    file_list = bmps_in_dir(dp, options.n, &found);
    /* found contains how many we actually found */
    if(file_list == NULL) {
        LOG_ERROR("Unable to open the files inside the directory\n");
        return EXIT_FAILURE;
    }

    if (found > options.n) {
        LOG_WARN("more than N = %d files were found, using N first files.\n", options.n);
    }

    /* Open n images */
    to_open = options.n;
    BMPImage ** bmp_list = open_files(file_list, to_open, options.dir);

    if (bmp_list == NULL) {
        LOG_ERROR("Error: Unable to open the required files (open_files).\n");
        free(file_list);
        return EXIT_FAILURE;
    }

    if (check_bmp_sizes(bmp_list, to_open)) {
        LOG_ERROR("Error: all images must have the same width and height.\n");
        bmp_free_list(bmp_list, to_open);
        return EXIT_FAILURE;
    }

    print_bmps_info(bmp_list, file_list, to_open);

    FILE * watermark_fd = fopen(options.watermark, "rb");

    if(watermark_fd == NULL) {
        LOG_ERROR("Unable to open watermark\n");
        return EXIT_FAILURE;
    }

    BMPImage * watermark = read_bmp(watermark_fd);

    FILE * secret_fd = fopen(options.secret, "rb");

    if(secret_fd == NULL) {
        LOG_ERROR("Unable to open secret image\n");
        return EXIT_FAILURE;
    }

    if(watermark == NULL) {
        LOG_ERROR("Error reading watermark image\n");
        return EXIT_FAILURE;
    }

    BMPImage * secret = read_bmp(secret_fd);

    if(secret == NULL) {
        LOG_ERROR("Error reading secret image\n");
        return EXIT_FAILURE;

    }

    if(watermark->header.image_size_bytes != secret->header.image_size_bytes) {
        LOG_ERROR("Watermark image must have same size as secret image\n");
        return EXIT_FAILURE;
    }

    if (check_shadow_sizes(secret, bmp_list, to_open, options.n, options.k)) {
        LOG_ERROR("Error: one or more of the shadow images does not have the required size.\n");
        free_bmp(secret);
        free(bmp_list);
        return EXIT_FAILURE;
    }

    distribute(options.k, options.n, secret, bmp_list, watermark);

    return EXIT_SUCCESS;

}

int recovery_mode(cmd_options options){
    DIR * dp = opendir(options.dir);

    if(dp == NULL) {
        LOG_ERROR("Unable to open directory\n");
        return EXIT_FAILURE;
    }

    char ** file_list = NULL;
    int found = 0, to_open = 0;

    /* Count should be n, we need n image shares */
    file_list = bmps_in_dir(dp, options.k, &found);
    /* found contains how many we actually found */

    if(file_list == NULL) {
        LOG_ERROR("Unable to open the files inside the directory\n");
        return EXIT_FAILURE;
    }


    if (found > options.k) {
        LOG_WARN("more than K = %d files were found, using K first files.\n", options.k);
    }

    /* Open n images */
    to_open = options.k;

    BMPImage ** bmp_list = open_files(file_list, to_open, options.dir);

    if (bmp_list == NULL) {
        LOG_ERROR("Error: Unable to open the required files (open_files).\n");
        free(file_list);
        return EXIT_FAILURE;
    }

    if (check_bmp_sizes(bmp_list, to_open)) {
        LOG_ERROR("Error: all images must have the same width and height.\n");
        bmp_free_list(bmp_list, to_open);
        return EXIT_FAILURE;
    }

    print_bmps_info(bmp_list, file_list, to_open);

    FILE * rw_fd = fopen(options.watermark, "rb");

    if(rw_fd == NULL) {
        LOG_ERROR("Unable to open watermark\n");
        return EXIT_FAILURE;
    }

    BMPImage * rw_bmp = read_bmp(rw_fd);

    if(rw_bmp == NULL) {
        LOG_ERROR("Error reading watermark image\n");
        return EXIT_FAILURE;
    }

    recover(options.k, options.n, options.secret, bmp_list, rw_bmp);

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

    logger_set_verbose(options.verbose);

    print_args_info(&options);

    if(options.mode == DISTRIBUTE_MODE){
        return distribute_mode(options);
    } else if (options.mode == RECOVER_MODE){
        return recovery_mode(options);
    }

    return EXIT_FAILURE;

}








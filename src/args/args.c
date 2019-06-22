//
// Created by Julian Palacci on 2019-06-12.
//

#include <args/args.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <logger/logger.h>

cmd_status parse_args(int argc, char *argv[], struct cmd_options *options) {
    init_options(options);

    static struct option long_options[] = {
                    {"dir",  required_argument, NULL, 'i'},
                    {"verbose", no_argument, NULL, 'v'},
                    {NULL, 0, NULL, 0}
    };

    int option_pos = 0;

    while (1) {
        int c;
        size_t len;

        c = getopt_long_only(argc, argv, "drs:m:k:n:", long_options, NULL);

        if (c == -1) {
            break;
        }

        if (arg_invalid_pos(c, option_pos)) {
            return ERROR_ORDER;
        }

        switch (c) {
            case 'd':
                if (options->mode) {
                    return ERROR_D_AND_R;
                }
                options->mode = DISTRIBUTE_MODE;
                break;

            case 'r':
                if (options->mode) {
                    return ERROR_D_AND_R;
                }
                options->mode = RECOVER_MODE;
                break;

            case 's':
                len = strlen(optarg);
                if (len > MAX_FILENAME_LEN - 1 || len == 0) {
                    return ERROR_NOSECRET;
                }

                strcpy(options->secret, optarg);
                break;

            case 'i':
                len = strlen(optarg);
                if (len > MAX_FILENAME_LEN - 1 || len == 0) {
                    return ERROR_NODIR;
                }

                strcpy(options->dir, optarg);
                break;

            case 'k':
                options->k = atoi(optarg);
                break;

            case 'n':
                options->n = atoi(optarg);
                if (options->n == 0) {
                    return ERROR_NON;
                }
                break;
            case 'm':
                len = strlen(optarg);
                if (len > MAX_FILENAME_LEN - 1 || len == 0) {
                    return ERROR_NOWATER;
                }

                strcpy(options->watermark, optarg);
                break;
            case 'v':
                options->verbose = true;
                break;
            default:
                return ERROR_GETOPT;
        }

        option_pos++;
    }

    if (!options->mode) {
        return ERROR_NOMODE;
    }
    else if (strlen(options->secret) == 0) {
        return ERROR_NOSECRET;
    }
    else if (strlen(options->watermark) == 0) {
        return ERROR_NOWATER;
    }
    else if (options->k == 0) {
        return ERROR_NOK;
    } else if (options->n == 0) {
        return ERROR_NON;
    }
    else if (strlen(options->watermark) == 0) {
        return ERROR_NODIR;
    }
    return CMD_SUCCESS;


}

void init_options(cmd_options * options) {

    options->mode = 0;
    options->secret[0] = 0;
    options->dir[0] = 0;
    options->watermark[0] = 0;
    options->k = 0;
    options->n = 0;
    options->verbose = false;

}

int arg_invalid_pos(int c, int pos) {
    return ((c == 'd' && pos != 0) ||
            (c == 'r' && pos != 0) ||
            (c == 's' && pos != 1) ||
            (c == 'm' && pos != 2) ||
            (c == 'k' && pos != 3) ||
            (c == 'n' && pos != 4) ||
            (c == 'i' && pos != 5));
}

int validate_args(struct cmd_options *options)
{

    if((options->k != 2 || options->n != 4) && (options->k != 4 || options->n != 8)) {
        LOG_ERROR("Valid schemes are (2,4) or (4,8)\n");
        return -1;
    }



    return 0;
}

void print_error(cmd_status status)
{
    switch (status)
    {
        case ERROR_D_AND_R:
            LOG_ERROR("Error: -r and -d cannot be defined simultaneously.\n");
            break;
        case ERROR_NOMODE:
            LOG_ERROR("Error: -r or -d must be defined.\n");
            break;
        case ERROR_NOK:
            LOG_ERROR("Error: K was invalid or was not specified.\n");
            break;
        case ERROR_NOSECRET:
            LOG_ERROR("Error: secret image was invalid or was not specified.\n");
            break;
        case ERROR_GETOPT:
            LOG_ERROR("Error: malformed argument detected.\n");
            break;
        case ERROR_NON:
            LOG_ERROR("Error: N was invalid or was not specified.\n");
            break;
        case ERROR_NODIR:
            LOG_ERROR("Error: directory was invalid or was not specified.\n");
            break;
        case ERROR_ORDER:
            LOG_ERROR("Error: arguments must be in the correct order.\n");
            break;
        default:
            LOG_ERROR("Unknown error.\n");
            break;
    }
}

void print_args_info(cmd_options *options)
{
    LOG_INFO("Mode: %s\n", options->mode == RECOVER_MODE ? "Recover" : "Distribute");
    LOG_INFO("Secret file: \"%s\"\n", options->secret);
    LOG_INFO("Watermark file: \"%s\"\n", options->watermark);
    LOG_INFO("K: %d\n", options->k);
    LOG_INFO("N: %d\n", options->n);
    LOG_INFO("Directory: \"%s\"\n", options->dir);
}





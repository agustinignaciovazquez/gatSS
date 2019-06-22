#ifndef ARGS_H
#define ARGS_H

#define MAX_FILENAME_LEN 255
#define RECOVER_MODE 1
#define DISTRIBUTE_MODE 2
#define MAX_FILENAME_LEN 255

#include <stdbool.h>

/* Enums to report status of parsing commands */

typedef enum cmd_status {
    CMD_SUCCESS, ERROR_D_AND_R, ERROR_NOMODE, ERROR_NOK,
    ERROR_NOSECRET, ERROR_GETOPT, ERROR_NON, ERROR_NODIR, ERROR_ORDER, ERROR_NOWATER
} cmd_status;

/* Command options */

typedef struct cmd_options {
    int mode;
    char secret[MAX_FILENAME_LEN];
    char watermark[MAX_FILENAME_LEN];
    int k;
    int n;
    char dir[MAX_FILENAME_LEN];
    bool verbose;
} cmd_options;

/* Parses the commands passed to main (argc, argv). Fills the structure cmd_options. Reports parsing status in cmd_status */
cmd_status parse_args(int argc, char *argv[], struct cmd_options *options);

/* Indicates whether an argument is in an incorrect position */
int arg_invalid_pos(int c, int pos);

/* Validates arguments. Checks if (k,n) scheme is (2,4) or (4,8). Returns a 1 if successful, 0 otherwise */
int validate_args(struct cmd_options *options);

/* Print cmd_status error if cmd_status is different to CMD_SUCCESS */
void print_error(cmd_status status);

/* Prints cmd_options structure. Information about the loaded arguments */
void print_args_info(cmd_options *options);

/* Inits the cmd_options structure */
void init_options(cmd_options * options);


#endif //MODLIB_ARGS_H

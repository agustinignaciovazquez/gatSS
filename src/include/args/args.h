#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

#define FILE_MAX_LEN 512

/* Status of input */

typedef enum commandStat {
    SUCCESS, ERROR_DUPMODE, ERROR_MODE, ERROR_NO_N_VALUE, ERROR_NO_K_VALUE, ERROR_NO_SECRET_FILE, ERROR_NO_WATERMARK_FILE, ERROR_NO_DIRECTORY
} commandStat;


typedef enum mode_t
{
	NON,
    DISTRIBUTE,
    RETRIEVE

} Mode;

/* Command options */
typedef struct options_t
{
    Mode mode;
    char secret_file_name [FILE_MAX_LEN];
    char water_mark_file_name [FILE_MAX_LEN];
    uint8_t  min_shadows_amount;
    uint8_t  total_amount_of_shadows ;
    char directory [FILE_MAX_LEN];

} Options;

/*Prints error status*/
void printError(commandStat status);

/* Parses the commands recieved by console. Initialize the struct Options. Checks for errors */
commandStat parseOptions(int argc, char *argv[], Options *options);

/* Verifies if the scheme (k,n) is (2,4) or (4,8) */
int validateArgs( Options *options);

/* Prints help */
void printHelp();

/* Inits the Options structure */
void paramInit(Options * options);

#endif //MODLIB_ARGS_H

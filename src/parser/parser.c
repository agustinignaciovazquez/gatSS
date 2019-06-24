//
// Created by smanganaro on 2019-06-23.
//

#include "parser.h"
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>


commandStat parseOptions(int argc, char **argv, Options * options){


  paramInit(options);

  int c;
  size_t arglen = 0;

  static struct option long_options[] =
        {
          {"dir",  required_argument, NULL, 'i'},
          {"des", no_argument, NULL, 'x'},/*RANDOM FIX*/
          {NULL, 0, NULL, 0}
        };
      /* getopt_long stores the option index here. */
      /*int option_index = 0;*/

  while (1)
    {
      c = getopt_long_only (argc, argv, "hdrs:m:k:n:",
                       long_options, NULL);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c) {
       /*
        case 0:
          If this option set a flag, do nothing else now.
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;*/

        case 'h':
          printHelp();
          exit(0);

        case 'd':
          if(options->mode){
            return ERROR_DUPMODE;
          }
          options->mode = DISTRIBUTE;
          printf("%d\n",options->mode);
          break;

        case 'r':
          if(options->mode){
            return ERROR_DUPMODE;            
          }
          options->mode = RETRIEVE;
          printf("%d\n",options->mode);
          break;

        case 's':
          arglen = strlen(optarg);
          if (arglen > FILE_MAX_LEN - 1 || arglen == 0) {
            return ERROR_NO_SECRET_FILE;
          }
          strcpy(options->secret_file_name, optarg);
          printf("%s\n",options->secret_file_name);
          break;

        case 'm':
          arglen = strlen(optarg);
          if (arglen > FILE_MAX_LEN - 1 || arglen == 0) {
            return ERROR_NO_WATERMARK_FILE;
          }
          strcpy(options->water_mark_file_name, optarg);
          printf("%s\n",options->water_mark_file_name);
          break;

        case 'k':
          options->min_shadows_amount = atoi(optarg);
          printf("%d\n",options->min_shadows_amount);
          break;

        case 'n':
          options->total_amount_of_shadows = atoi(optarg);
          printf("%d\n",options->total_amount_of_shadows);
          break;

        case 'i':
          arglen = strlen(optarg);
          if (arglen > FILE_MAX_LEN - 1 || arglen == 0) {
            return ERROR_NO_DIRECTORY;
          }
          strcpy(options->directory, optarg);
          printf("%s\n",options->directory);
        break;


        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort ();
        }
    }

    if (options->min_shadows_amount == -1)
    {
        return ERROR_NO_K_VALUE;
    }

    if (options->total_amount_of_shadows == -1)
    {
        return ERROR_NO_N_VALUE;
    }

    if (!options->mode) {
        printf("%d\n",options->mode);
        return ERROR_MODE;
    }

    return SUCCESS;
}


void printHelp(){

    printf("Usage: stegosharedsecret [OPTIONS]\n");
    printf("Options:\n");
    printf("%-30s", "\t-h");
    printf("prints the help and quits.\n");
    printf("%-30s", "\t-d ");
    printf("selects distribution mode.\n");
    printf("%-30s", "\t-r ");
    printf("selects retrieve mode.\n");
    printf("%-30s", "\t-dir path");
    printf("if -d, path to where you want to save the new images. if -r, path to where the images with the hidden secret are. \n");
    printf("%-30s", "\t-k number");
    printf("minimum amount of shadows to retrieve a secret. \n");
    printf("%-30s", "\t-n number");
    printf("total amount of shadows in which the secret will be distributed.\n");
    printf("%-30s", "\t-s image");
    printf("if -d, name of the input .bmp file. if -r, name of the output .bmp file.\n");
    printf("%-30s", "\t-m image");
    printf("if -d, name of the watermark input .bmp file. if -r, name of the watermark output .bmp file \n");
    printf("%-30s", "\t-pass password");
}

void paramInit(Options * options){

    options->mode = 0;
    options->secret_file_name[0] = 0;
    options->water_mark_file_name[0] = 0;
    options->min_shadows_amount = -1;
    options->total_amount_of_shadows = -1;
    options->directory[0] = 0;

}

int validateArgs(Options *options){


    if((options->min_shadows_amount != 2 || options->total_amount_of_shadows != 4) && (options->min_shadows_amount != 4 || options->total_amount_of_shadows != 8)) {
        printf("%s\n","Invalid scheme");
        printf("%s\n","Please choose between (2,4) or (4,8)");

        return -1;
    }

    return 0;
}


void printError(commandStat status){

  switch(status){

    case ERROR_DUPMODE:
      fprintf(stderr, "ERROR: You can only choose one mode at a time.\n");
      break;

    case ERROR_MODE:
      fprintf(stderr, "ERROR: You must specify a valid execution mode.\n");
      break;

    case ERROR_NO_N_VALUE:
      fprintf(stderr, "ERROR: You must specify total amount of shadows in which the secret is distributed.\n");


    case ERROR_NO_K_VALUE:
      fprintf(stderr, "ERROR: You must specify a minimum amount of shadows needed to retrieve a secret.\n");
      break;  

    case ERROR_NO_SECRET_FILE:
      fprintf(stderr, "ERROR: You must specify a valid secret file. No more than 512 characters.\n");
      break;

    case ERROR_NO_WATERMARK_FILE:
      fprintf(stderr, "ERROR: You must specify a valid watermark file. No more than 512 characters.\n");
      break;

    case ERROR_NO_DIRECTORY:
      fprintf(stderr, "ERROR: You must specify a valid directory for the secret file.\n");  
      break;  

    case SUCCESS:
      break;

  }
  
}

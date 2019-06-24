#include "parser.h"
#include "utils.h"
#include "wrapper.h"
#include <time.h>

#include <stdlib.h>

int main(int argc, char* argv[]) {
    Options options;
    commandStat status = parseOptions(argc, argv, &options);

    if(status != SUCCESS) {
        printError(status);
        return EXIT_FAILURE;
    }
    if (validateArgs(&options)){
        return EXIT_FAILURE;
    }

    rand_set_seed(time(NULL));
    if(options.mode == DISTRIBUTE){
        return distribute_mode(options);
    } else if (options.mode == RETRIEVE){
        return recovery_mode(options);
    }

    return EXIT_FAILURE;

}








#include <stdarg.h>
#include <stdio.h>
#include <logger/logger.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


static bool verbose = false;

void logger_set_verbose(bool value) {
  verbose = value;
}

int logger_log(logger_level_t level, const char *fmt, ...) {
    if (!verbose && level < LOGGER_ERROR) {
        return 0;
    }

    va_list args;
    va_start(args, fmt);
    int ret = 0;
    
    switch(level) {
      case LOGGER_DEBUG: 
        printf(ANSI_COLOR_CYAN"[DEBUG] "ANSI_COLOR_RESET);
        vprintf(fmt, args);
        break;
      case LOGGER_INFO: 
        printf(ANSI_COLOR_GREEN"[INFO] "ANSI_COLOR_RESET);
        vprintf(fmt, args);
        break;
      case LOGGER_WARN:
        printf(ANSI_COLOR_YELLOW"[WARN] "ANSI_COLOR_RESET);
        vprintf(fmt, args);
        break;
      case LOGGER_ERROR:
        fprintf(stderr, ANSI_COLOR_RED"[ERROR] "ANSI_COLOR_RESET);
        vfprintf(stderr, fmt, args);
        break;
    }
    
    va_end(args);
    return ret;
}

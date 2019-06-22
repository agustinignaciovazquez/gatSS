#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdbool.h>

typedef enum {
  LOGGER_DEBUG,
  LOGGER_INFO,
  LOGGER_WARN,
  LOGGER_ERROR
} logger_level_t;

#define LOG_DEBUG(...) logger_log(LOGGER_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) logger_log(LOGGER_INFO, __VA_ARGS__)
#define LOG_WARN(...) logger_log(LOGGER_WARN, __VA_ARGS__)
#define LOG_ERROR(...) logger_log(LOGGER_ERROR, __VA_ARGS__)

int logger_log(logger_level_t level, const char *fmt, ...);
void logger_set_verbose(bool value);

#endif // LOGGER_H_

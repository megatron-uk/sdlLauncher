#pragma once
// Logging levels
enum loglevel_t { DEBUG, INFO, WARNING, ERROR };
extern enum loglevel_t loglevel;

int log_debug(FILE *fp, const char *format, ...);
int log_info(FILE *fp, const char *format, ...);
int log_warn(FILE *fp, const char *format, ...);
int log_error(FILE *fp, const char *format, ...);
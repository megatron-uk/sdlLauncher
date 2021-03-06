#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "logging.h"

#define LOG_FORMAT	"[%s]\t: %s"  // loglevel: message\n

// From: https://gist.github.com/rumpelsepp/3229275bbe9e3f41ba5f33a84643fe73
static int __log(FILE *fp, const char *level, const char *format, va_list args) {
	int len;
	char *newformat;
	char *logformat = LOG_FORMAT;

	len = snprintf(NULL, 0, logformat, level, format);
	len++;  // Trailing null byte.

	newformat = malloc(len);
	if (newformat == NULL){
		return -1;	
	}

	len = snprintf(newformat, len, logformat, level, format);
	if (len < 0) {
		goto cleanup;
	}

	len = vfprintf(fp, newformat, args);

cleanup:
	free(newformat);
	return len;
}

// Debug logs are flushed to disk
int log_debug(FILE *fp, const char *format, ...) {
	int r = 0;
	
	// Enable synchronous logging of debug messages
#ifdef LOGGING
	va_list ap;
	va_start(ap, format);
	r = __log(fp, "DEBUG", format, ap);
	va_end(ap);
	fflush(fp);
#endif
	return r;
}

int log_info(FILE *fp, const char *format, ...) {
	int r;
	va_list ap;

	va_start(ap, format);
	r = __log(fp, "INFO", format, ap);
	va_end(ap);

	return r;
}

int log_warn(FILE *fp, const char *format, ...) {
	int r;
	va_list ap;

	va_start(ap, format);
	r = __log(fp, "WARN", format, ap);
	va_end(ap);

	return r;
}

// Errors are flushed to disk
int log_error(FILE *fp, const char *format, ...) {
	int r;
	va_list ap;

	va_start(ap, format);
	r = __log(fp, "ERROR", format, ap);
	va_end(ap);
	// Errors should *always* be flushed to disk
	fflush(fp);

	return r;
}
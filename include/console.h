#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#ifdef DEBUG

#include <stdio.h>
#define CONSOLE_DEBUG(log_format, log_args...)   printf("DEBUG at [%s:%d]: " log_format "\n", __FILE__, __LINE__,##log_args)
#define CONSOLE_INFO(log_format, log_args...)    printf("INFO: " log_format "\n", ##log_args)
#define CONSOLE_WARNING(log_format, log_args...) fprintf(stderr, "WARNING at [%s:%d]: " log_format "\n", __FILE__, __LINE__,##log_args)
#define CONSOLE_ERROR(log_format, log_args...)   fprintf(stderr, "ERROR at [%s:%d]: " log_format "\n", __FILE__, __LINE__,##log_args)

#else

#define CONSOLE_DEBUG(log_format, log_args...)
#define CONSOLE_INFO(log_format, log_args...)
#define CONSOLE_WARNING(log_format, log_args...)
#define CONSOLE_ERROR(log_format, log_args...)

#endif

#endif // _CONSOLE_H_
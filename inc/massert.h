#ifndef SENSEY_ASSERT
#define SENSEY_ASSERT

#include "logger.h"

#define massert(conditions, message) \
    if (!(conditions)) { \
		logger_fatal("%s - Error in file %s at line %lu\n", message, __FILE__, (unsigned long) __LINE__);\
        fprintf(stderr, "%s - Error in file %s at line %lu\n", message, __FILE__, (unsigned long) __LINE__); \
        exit(EXIT_FAILURE); \
    }

#endif

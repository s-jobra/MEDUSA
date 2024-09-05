/**
 * @file error.h
 * @brief Custom error handling functions
 */

#include <stdlib.h>

#ifndef ERROR_H
#define ERROR_H

/**
 * Function that handles program exiting in case of error.
 * 
 * @param error Error message format.
*/
void error_exit(const char *error, ...);

/**
 * Custom malloc function including error handling.
 */
void* my_malloc(size_t size);

/**
 * Custom calloc function including error handling.
 */
void* my_calloc(size_t n, size_t elem_size);

/**
 * Custom realloc function including error handling.
 */
void* my_realloc(void *p, size_t size);

#endif
/* end of "error.h" */
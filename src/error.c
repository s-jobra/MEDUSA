#include "error.h"

void error_exit(const char *error, ...)
{
    va_list args;
    va_start(args, error);
    
    // create error message format
    unsigned error_len = strlen(error) + strlen(ERROR_TEXT);
    char msg[error_len + 1];
    strcpy(msg, ERROR_TEXT);
    strcat(msg, error);

    vfprintf(stderr, msg, args);
    va_end(args);

    exit(1);
}

void* my_malloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        error_exit("Bad memory allocation.\n");
    }
    return p;
}

void* my_realloc(void *p, size_t size) {
    void *p_new = realloc(p, size);
    if (p_new == NULL) {
        error_exit("Memory reallocation failed.\n");
    }
    return p_new;
}

/* end of "error.c" */
#include "error.h"

void error_exit(const char *error)
{
    // create error message format
    unsigned error_len = strlen(error) + strlen(ERROR_TEXT);
    char msg[error_len + 1];
    strcpy(msg, ERROR_TEXT);
    strcat(msg, error);

    fprintf(stderr, "%s\n",msg);
    exit(1);
}

void* my_malloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        error_exit("Bad memory allocation.");
    }
    return p;
}

void* my_realloc(void *p, size_t size) {
    void *p_new = realloc(p, size);
    if (p_new == NULL) {
        error_exit("Memory reallocation failed.");
    }
    return p_new;
}

/* end of "error.c" */
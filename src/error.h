#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ERROR_H
#define ERROR_H

/** Beginning of an error message. */
#define ERROR_TEXT "ERROR: "

/**
 * Function that handles program exiting in case of error.
 * 
 * @param error Error message.
*/
void error_exit(const char *error);

#endif
/* end of "error.h" */
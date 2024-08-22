/**
 * @file spec_parse.h
 * @brief Module for parsing and initializing from the specification language
 */

#include <stdio.h>
#include <sylvan.h>

#ifndef SPEC_PARSE_H
#define SPEC_PARSE_H

/**
 * Function for Sylvan setup for verification purposes
 */
void init_sylvan_verif();

/**
 * Initializes the circuit MTBDD from the specification file.
 * @param spec already opened file with initial specification
 * @param circ MTBDD that will be initialized
 */
void init_from_spec(FILE *spec,  MTBDD *circ);

#endif
/* end of "spec_parse.h" */
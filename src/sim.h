#include <ctype.h>  // For isspace(), isdigit()
#include <stdbool.h>

#include "custom_mtbdd.h"
#include "gates.h"
#include "error.h"

#ifndef SIMULATOR_H
#define SIMULATOR_H

#define CMD_MAX_LEN 10 // Max. supported length of qasm command
#define Q_ID_MAX_LEN 10 // Max. number of characters in qubit identificator

/**
 * Function parses a given QASM file and simulates it.
 * 
 * @param in input QASM file
 * 
 * @param circ custom MTBDD that will represent end circuit state
 */
void sim_file(FILE *in, MTBDD *circ);

#endif
/* end of "sim.h" */

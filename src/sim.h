#include <ctype.h>  // For isspace(), isdigit()
#include <stdbool.h>
#include <errno.h>

#include "qparam.h"
#include "mtbdd.h"
#include "gates.h"
#include "mtbdd_symb_val.h"
#include "gates_symb.h"
#include "symb_utils.h"
#include "error.h"
#include "htab.h"

#ifndef SIMULATOR_H
#define SIMULATOR_H

#define CMD_MAX_LEN 10 // Max. supported length of qasm command
#define NUM_MAX_LEN 25 // Max. number of characters in a parsed number

/**
 * Parses a given QASM file and simulates this circuit
 * 
 * @param in input QASM file
 * 
 * @param circ custom MTBDD that will represent end circuit state
 * 
 * @param n_qubits number of qubits in the circuit
 * 
 * @param bits_to_measure array for storing the qubits that are to be measured
 * 
 * @param is_measure true if some measure operation is present
 * 
 * @param is_symb should perform symbolic simulation
 * 
 */
void sim_file(FILE *in, MTBDD *circ, int *n_qubits, int **bits_to_measure, bool *is_measure, bool opt_symb);

/**
 * Measures all bits in the given array (compatible only with measurement at the end of the circuit)
 * 
 * @param samples the total number of samples
 * 
 * @param circ MTBDD representation of the end circuit state
 * 
 * @param n number of qubits in the circuit
 * 
 * @param bits_to_measure array for storing the qubits that are to be measured
 * 
 */
void measure_all(unsigned long samples, FILE *output, MTBDD circ, int n, int *bits_to_measure);

#endif
/* end of "sim.h" */

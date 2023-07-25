#include "mtbdd_symb_val.h"
#include "gates.h"

#ifndef GATES_SYMB_H
#define GATES_SYMB_H

/**
 * Apply gate X on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_symb_x(MTBDD *p_t, uint32_t xt);

/**
 * Apply Hadamard gate on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic MTBDD
 * 
 * @param xt target qubit index
 */
void gate_symb_h(MTBDD *p_t, uint32_t xt);

#endif
/* end of "gates_symb.h" */
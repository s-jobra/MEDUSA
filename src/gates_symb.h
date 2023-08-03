#include "mtbdd_symb_val.h"
#include "gates.h"

#ifndef GATES_SYMB_H
#define GATES_SYMB_H

/**
 * Apply gate X on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_symb_x(MTBDD *p_t, uint32_t xt);

/**
 * Apply gate Y on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_symb_y(MTBDD *p_t, uint32_t xt);

/**
 * Apply gate Z on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_symb_z(MTBDD *p_t, uint32_t xt);

/**
 * Apply gate S on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_symb_s(MTBDD *p_t, uint32_t xt);

/**
 * Apply gate T on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_symb_t(MTBDD *p_t, uint32_t xt);

/**
 * Apply Hadamard gate on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 */
void gate_symb_h(MTBDD *p_t, uint32_t xt);

/**
 * Apply Controlled NOT gate on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param xc control qubit index
 */
void gate_symb_cnot(MTBDD *p_t, uint32_t xt, uint32_t xc);

/**
 * Apply Controlled Z gate on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param xc control qubit index
 */
void gate_symb_cz(MTBDD *p_t, uint32_t xt, uint32_t xc);

/**
 * Apply Toffoli gate on the symbolic state vector.
 * 
 * @param p_t pointer to a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param xc1 first control qubit index
 * 
 * @param xc2 second control qubit index
 */
void gate_symb_toffoli(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2);

#endif
/* end of "gates_symb.h" */
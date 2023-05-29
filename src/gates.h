#include "custom_mtbdd.h"

#ifndef GATES_H
#define GATES_H

/**
 * Permutation based implementation of the X gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, m_gate_x, MTBDD, uint64_t);

/**
 * Permutation based implementation of the Y gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, m_gate_y, MTBDD, uint64_t);

/**
 * Permutation based implementation of the Z gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, m_gate_z, MTBDD, uint64_t);

/**
 * Permutation based implementation of the S gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, m_gate_s, MTBDD, uint64_t);

/**
 * Permutation based implementation of the T gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, m_gate_t, MTBDD, uint64_t);

/**
 * Returns the probability the given qubit's state will be 1.
 * This implementation supports only a measurement of all the qubits at the end of the circuit.
 * 
 * @param a pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param curr_state current state vector (determined by previous measurements)
 * 
 * @param n number of qubits in the circuit
 * 
 */
prob_t measure(MTBDD* a, uint32_t xt, char *curr_state, int n);

/**
 * Apply quantum gate X on the state vector.
 * 
 * @param a pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_x(MTBDD* a, uint32_t xt);

/**
 * Apply quantum gate Y on the state vector.
 * 
 * @param a pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_y(MTBDD* a, uint32_t xt);

/**
 * Apply quantum gate Z on the state vector.
 * 
 * @param a pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_z(MTBDD* a, uint32_t xt);

/**
 * Apply quantum gate S on the state vector.
 * 
 * @param a pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_s(MTBDD* a, uint32_t xt);

/**
 * Apply quantum gate T on the state vector.
 * 
 * @param a pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_t(MTBDD* a, uint32_t xt);

/**
 * Function implementing quantum Hadamard gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
void gate_h(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum Rx(π/2) gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
void gate_rx_pihalf(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum Ry(π/2) gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
void gate_ry_pihalf(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum Controlled NOT gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param xc control qubit index
 */
void gate_cnot(MTBDD* p_t, uint32_t xt, uint32_t xc);

/**
 * Function implementing quantum Controlled Z gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param xc control qubit index
 */
void gate_cz(MTBDD* p_t, uint32_t xt, uint32_t xc);

/**
 * Function implementing quantum Toffoli gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param xc1 first control qubit index
 * 
 * @param xc2 second control qubit index
 */
void gate_toffoli(MTBDD* p_t, uint32_t xt, uint32_t xc1, uint32_t xc2);

/**
 * Function implementing quantum Fredkin gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt1 first target qubit index
 * 
 * @param xt2 second target qubit index
 * 
 * @param xc control qubit index
 */
void gate_fredkin(MTBDD* p_t, uint32_t xt1, uint32_t xt2, uint32_t xc);

#endif
/* end of "gates.h" */
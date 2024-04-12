#include <stdbool.h>
#include "mtbdd.h"
#include "qparam.h"

#ifndef GATES_H
#define GATES_H

/**
 * Permutation based implementation of the X gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, _gate_x, MTBDD, uint64_t);

LACE_TYPEDEF_CB(MTBDD, mtbdd_apply_gate_op, MTBDD, uint32_t);
TASK_DECL_3(MTBDD, mtbdd_apply_gate, MTBDD, mtbdd_apply_gate_op, uint32_t);
/**
 * Apply a gate operation <op> to <dd>. Custom apply needed because xt nodes may not be present in the reduced <dd>.
 * Otherwise it's basically the standard uapply.
 */
#define my_mtbdd_apply_gate(dd, op, param) RUN(mtbdd_apply_gate, dd, op, param)

TASK_DECL_5(MTBDD, mtbdd_apply_cgate, MTBDD, mtbdd_apply_gate_op, uint32_t, uint32_t, bool);
/**
 * Apply a controlled gate rotation operation <op> to <dd>. Custom apply needed because xc, xt nodes may not be present in the reduced <dd>.
 * Otherwise it's basically the standard uapply. Assumes xc < xt.
 * 
 * Note that in the current implementation will work only with a single controlled rotation operation because
 * of insufficient number of arguments for Sylvan caching.
 */
#define my_mtbdd_apply_cgate(dd, op, xc, xt) RUN(mtbdd_apply_cgate, dd, op, xc, xt, false)

/**
 * Returns the probability the given qubit's state will be 1.
 * This implementation supports only a measurement of all the qubits at the end of the circuit.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param curr_state current state vector (determined by previous measurements)
 * 
 * @param n number of qubits in the circuit
 * 
 */
prob_t measure(MTBDD *p_t, uint32_t xt, char *curr_state, int n);

/**
 * Apply quantum gate X on the state vector.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_x(MTBDD *p_t, uint32_t xt);

/**
 * Apply quantum gate Y on the state vector.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_y(MTBDD *p_t, uint32_t xt);

/**
 * Apply quantum gate Z on the state vector.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_z(MTBDD *p_t, uint32_t xt);

/**
 * Apply quantum gate S on the state vector.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_s(MTBDD *p_t, uint32_t xt);

/**
 * Apply quantum gate T on the state vector.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
void gate_t(MTBDD *p_t, uint32_t xt);

/**
 * Function implementing quantum Hadamard gate for a given MTBDD.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 */
void gate_h(MTBDD *p_t, uint32_t xt);

/**
 * Function implementing quantum Rx(π/2) gate for a given MTBDD.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 */
void gate_rx_pihalf(MTBDD *p_t, uint32_t xt);

/**
 * Function implementing quantum Ry(π/2) gate for a given MTBDD.
 * 
 * @param p_t pointer to an MTBDD
 * 
 * @param xt target qubit index
 */
void gate_ry_pihalf(MTBDD *p_t, uint32_t xt);

/**
 * Function implementing quantum Controlled NOT gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param xc control qubit index
 */
void gate_cnot(MTBDD *p_t, uint32_t xt, uint32_t xc);

/**
 * Function implementing quantum Controlled Z gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param xc control qubit index
 */
void gate_cz(MTBDD *p_t, uint32_t xt, uint32_t xc);

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
void gate_toffoli(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2);

/**
 * Function implementing quantum Multicontrol NOT gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param qparams list of all the target + control qubit indices (first index is assumed to be the target index)
 * 
 */
void gate_mcx(MTBDD *p_t, qparam_list_t *qparams);

#endif
/* end of "gates.h" */
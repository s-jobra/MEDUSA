#include "custom_mtbdd.h"

#ifndef GATES_H
#define GATES_H


/**
 * Function implementing quantum gate X for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
//void gate_x(MTBDD* p_t, uint32_t xt); FIXME:

/**
 * Function implementing quantum gate Y for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
void gate_y(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum gate Z for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
void gate_z(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum Hadamard gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
void gate_h(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum S gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
void gate_s(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum T gate for a given MTBDD.
 * 
 * @param p_t custom MTBDD
 * 
 * @param xt target qubit index
 */
void gate_t(MTBDD* p_t, uint32_t xt);

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
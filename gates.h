#include "custom_mtbdd.h"

#ifndef GATES_H
#define GATES_H

/**
 * Function implementing quantum gate X for a given MTBDD.
 */
MTBDD gate_x(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum gate Y for a given MTBDD.
 */
MTBDD gate_y(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum gate Z for a given MTBDD.
 */
MTBDD gate_z(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum Hadamard gate for a given MTBDD.
 */
MTBDD gate_h(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum S gate for a given MTBDD.
 */
MTBDD gate_s(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum T gate for a given MTBDD.
 */
MTBDD gate_t(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum Rx(π/2) gate for a given MTBDD.
 */
MTBDD gate_rx_pihalf(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum Ry(π/2) gate for a given MTBDD.
 */
MTBDD gate_ry_pihalf(MTBDD* p_t, uint32_t xt);

/**
 * Function implementing quantum Controlled NOT gate for a given MTBDD.
 */
MTBDD gate_cnot(MTBDD* p_t, uint32_t xt, uint32_t xc);

/**
 * Function implementing quantum Controlled Z gate for a given MTBDD.
 */
MTBDD gate_cz(MTBDD* p_t, uint32_t xt, uint32_t xc);

/**
 * Function implementing quantum Toffoli gate for a given MTBDD.
 */
MTBDD gate_toffoli(MTBDD* p_t, uint32_t xt, uint32_t xc1, uint32_t xc2);

/**
 * Function implementing quantum Fredkin gate for a given MTBDD.
 */
MTBDD gate_fredkin(MTBDD* p_t, uint32_t xt1, uint32_t xt2, uint32_t xc);

#endif
/* end of "gates.h" */
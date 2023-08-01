#include <z3.h>
#include "symbolic_tree.h"

#ifndef SOLVER_H
#define SOLVER_H

/**
 * Returns true if the two expressions are equal
 * 
 * @param t_a pointer to an expression
 *
 * @param t_b pointer to an expression
 * 
 * @param nvars total number of variables used in expressions
 * 
 */
bool solve_is_equal(stree_t *t_a, stree_t *t_b, size_t nvars);

#endif
/* end of "solver.h" */
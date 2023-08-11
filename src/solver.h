#include "stdlib.h"
#include <string.h>
#include <z3.h>
#include "symbolic_tree_types.h"
#include "error.h"

#ifndef SOLVER_H
#define SOLVER_H

/**
 * Returns true if the two expressions are equal
 */
bool expr_is_equal(stree_t *t_a, stree_t *t_b, size_t nvars);

#endif
/* end of "solver.h" */
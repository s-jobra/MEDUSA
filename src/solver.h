#include <z3.h>
#include "symbolic_tree.h"

#ifndef SOLVER_H
#define SOLVER_H

/**
 * Type for encapsulating solver data needed by all functions working with the solver
 */
typedef struct solver_data {
    Z3_ast *vars;
    Z3_context ctx;
    Z3_sort sort;
} solver_data_t;

/**
 * Type for the solver's internal representation of expressions
 */
typedef Z3_ast solver_exp_t;

/**
 * Type for a solver instance
 */
typedef Z3_solver solver_t;

/**
 * Initializes the given solver_data structure
 */
void solver_data_init(solver_data_t *sd, size_t nvars);

/**
 * Deletes the solver_data structure
 */
void solver_data_delete(solver_data_t *sd);

/**
 * Converts the symbolic tree to solver's internal tree
 */
solver_exp_t parse_stree(stree_t *t, solver_data_t *sdata);

/**
 * Creates a != b
 */
solver_exp_t solver_create_neq(solver_data_t *sdata, solver_exp_t a, solver_exp_t b);

/**
 * Returns whether the given query is solvable
 */
bool solve(solver_data_t *sdata, solver_exp_t query);

#endif
/* end of "solver.h" */
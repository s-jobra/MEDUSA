#include "solver.h"

/**
 * Type for encapsulating solver data needed by all functions working with the solver
 */
typedef struct solver_data {
    Z3_ast *vars;
    Z3_context ctx;
    Z3_sort sort;
} solver_data_t;

/**
 * Initializes the solver_data structure
 */
static void solver_data_init(solver_data_t *sd, size_t nvars)
{
    sd->vars = my_malloc(sizeof(Z3_ast) * nvars);
    for (int i = 0; i < nvars; i++) {
        sd->vars[i] = NULL;
    }

    Z3_config cfg = Z3_mk_config();
    sd->ctx = Z3_mk_context(cfg);
    Z3_del_config(cfg);

    sd->sort = Z3_mk_int_sort(sd->ctx);
}

/**
 * Deletes the solver_data structure
 */
static void solver_data_delete(solver_data_t *sd)
{
    free(sd->vars);
    Z3_del_context(sd->ctx);
}

/**
 * Creates new solver constant
 */
static void solver_init_var(stree_t *t, solver_data_t *sdata)
{
    sdata->vars[t->val->var] = Z3_mk_fresh_const(sdata->ctx, NULL, sdata->sort);
}

/**
 * Returns the solver representation of the tree's variable and its coefficient
 */
static Z3_ast solver_get_var(stree_t *t, solver_data_t *sdata)
{
    if (sdata->vars[t->val->var] == NULL) {
        solver_init_var(t, sdata);
    }

    Z3_ast args[2];
    args[0] = Z3_mk_int64(sdata->ctx, mpz_get_si(t->val->coef), sdata->sort);
    args[1] = sdata->vars[t->val->var];
    Z3_ast res = Z3_mk_mul(sdata->ctx, 2, args);
    return res;
}

/**
 * Converts the symbolic tree to solver's internal tree
 */
static Z3_ast parse_stree(stree_t *t, solver_data_t *sdata)
{
    Z3_ast res;

    if(t->type == ST_VAL) {
        res = solver_get_var(t, sdata);
    }
    else {
        Z3_ast args[2];
        args[0] = parse_stree(t->ls, sdata);
        args[1] = parse_stree(t->rs, sdata);

        switch (t->type) {
            case ST_ADD:
                res = Z3_mk_add(sdata->ctx, 2, args);
                break;
            case ST_SUB:
                res = Z3_mk_sub(sdata->ctx, 2, args);
                break;
            case ST_MUL:
                res = Z3_mk_mul(sdata->ctx, 2, args);
                break;
        }
    }
    return res;
}

/**
 * Returns whether the given query is solvable
 */
static bool solve(solver_data_t *sdata, Z3_ast query)
{
    bool res;
    Z3_solver s = Z3_mk_solver(sdata->ctx);
    Z3_solver_assert(sdata->ctx, s, query);
    if (Z3_solver_check(sdata->ctx, s) == Z3_L_FALSE) {
        res = true;
    }
    else {
        res = false;
    }
    return res;
}

bool solve_is_equal(stree_t *t_a, stree_t *t_b, size_t nvars)
{
    solver_data_t sdata;
    solver_data_init(&sdata, nvars);

    Z3_ast res_a = parse_stree(t_a, &sdata);
    Z3_ast res_b = parse_stree(t_b, &sdata);
    Z3_ast eq = Z3_mk_eq(sdata.ctx, res_a, res_b);
    Z3_ast query = Z3_mk_not(sdata.ctx, eq);

    bool is_equal = solve(&sdata, query);

    solver_data_delete(&sdata);

    return is_equal;
}

/* end of "solver.c" */
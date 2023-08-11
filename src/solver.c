#include "solver.h"

// =============
// Z3 interface:
// =============

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
static void solver_data_init(solver_data_t *sd, size_t nvars)
{
    sd->vars = my_malloc(sizeof(Z3_ast) * nvars);
    memset(sd->vars, 0, sizeof(Z3_ast) * nvars);

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
 * Creates a new solver instance
 */
static solver_t solver_create(solver_data_t *sdata)
{
    solver_t s = Z3_mk_solver(sdata->ctx);
    Z3_solver_inc_ref(sdata->ctx, s);
    return s;
}

/**
 * Deletes the given solver instance
 */
static void solver_delete(solver_data_t *sdata, solver_t s)
{
    Z3_solver_dec_ref(sdata->ctx, s);
}

/**
 * Creates a new solver constant (0-ary function)
 */
static void solver_init_var(stree_t *t, solver_data_t *sdata)
{
    sdata->vars[t->val->var] = Z3_mk_fresh_const(sdata->ctx, NULL, sdata->sort);
}

/**
 * Returns the solver representation of the tree's variable and its coefficient
 */
static solver_exp_t solver_get_var(stree_t *t, solver_data_t *sdata)
{
    if (sdata->vars[t->val->var] == NULL) {
        solver_init_var(t, sdata);
    }

    solver_exp_t args[2];
    args[0] = Z3_mk_int64(sdata->ctx, mpz_get_si(t->val->coef), sdata->sort);
    args[1] = sdata->vars[t->val->var];
    solver_exp_t res = Z3_mk_mul(sdata->ctx, 2, args);
    return res;
}

/**
 * Asserts the query and checks the result
 */
static bool solver_assert(solver_t s, solver_data_t *sdata, solver_exp_t query)
{
    bool res;
    Z3_solver_assert(sdata->ctx, s, query);
    if (Z3_solver_check(sdata->ctx, s) == Z3_L_FALSE) {
        res = true;
    }
    else {
        res = false;
    }
    return res;
}

/**
 * Creates a != b
 */
static solver_exp_t solver_create_neq(solver_data_t *sdata, solver_exp_t a, solver_exp_t b)
{
    solver_exp_t eq = Z3_mk_eq(sdata->ctx, a, b);
    solver_exp_t neq = Z3_mk_not(sdata->ctx, eq);
    return neq;
}

/**
 * Converts the symbolic tree to solver's internal tree
 */
static solver_exp_t parse_stree(stree_t *t, solver_data_t *sdata)
{
    solver_exp_t res;

    if(t->type == ST_VAL) {
        res = solver_get_var(t, sdata);
    }
    else {
        solver_exp_t args[2];
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

// =======================================================

/**
 * Returns whether the given query is solvable
 */
static bool solve(solver_data_t *sdata, solver_exp_t query)
{
    solver_t s = solver_create(sdata);
    bool res = solver_assert(s, sdata, query);
    solver_delete(sdata, s);
    return res;
}

bool expr_is_equal(stree_t *t_a, stree_t *t_b, size_t nvars)
{
    solver_data_t sdata;
    solver_data_init(&sdata, nvars);

    solver_exp_t res_a = parse_stree(t_a, &sdata);
    solver_exp_t res_b = parse_stree(t_b, &sdata);
    solver_exp_t query = solver_create_neq(&sdata, res_a, res_b);

    bool is_equal = solve(&sdata, query);

    solver_data_delete(&sdata);

    return is_equal;
}

/* end of "solver.c" */
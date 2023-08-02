#include "solver.h"

// =============
// Z3 interface:
// =============

void solver_data_init(solver_data_t *sd, size_t nvars)
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

void solver_data_delete(solver_data_t *sd)
{
    free(sd->vars);
    Z3_del_context(sd->ctx);
}

static solver_t solver_create(solver_data_t *sdata)
{
    solver_t s = Z3_mk_solver(sdata->ctx);
    Z3_solver_inc_ref(sdata->ctx, s);
    return s;
}

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

solver_exp_t solver_create_neq(solver_data_t *sdata, solver_exp_t a, solver_exp_t b)
{
    solver_exp_t eq = Z3_mk_eq(sdata->ctx, a, b);
    solver_exp_t neq = Z3_mk_not(sdata->ctx, eq);
    return neq;
}

solver_exp_t parse_stree(stree_t *t, solver_data_t *sdata)
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

bool solve(solver_data_t *sdata, solver_exp_t query)
{
    solver_t s = solver_create(sdata);
    bool res = solver_assert(s, sdata, query);
    solver_delete(sdata, s);
    return res;
}

/* end of "solver.c" */
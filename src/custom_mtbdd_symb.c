#include "custom_mtbdd_symb.h"

/*
 * Custom leaf implementation is taken from: https://github.com/MichalHe/sylvan-custom-leaf-example
 */

uint32_t ltype_s_id;

/* SETUP */
void init_my_leaf_symb()
{
    ltype_s_id = sylvan_mt_create_type();

    sylvan_mt_set_create(ltype_s_id, my_leaf_symb_create);
    sylvan_mt_set_destroy(ltype_s_id, my_leaf_symb_destroy);
    sylvan_mt_set_equals(ltype_s_id, my_leaf_symb_equals);
    sylvan_mt_set_hash(ltype_s_id, my_leaf_symb_hash);
}

/* CUSTOM HANDLES */
void my_leaf_symb_create(uint64_t *ldata_p_raw)
{
    lsymb_t** ldata_p = (lsymb_t**)ldata_p_raw; // Leaf data type is uint64_t, we store there ptr to our actual data
    
    lsymb_t* orig_ldata = *ldata_p;
    lsymb_t* new_ldata = (lsymb_t*)my_malloc(sizeof(lsymb_t));

    new_ldata->a = st_init(orig_ldata->a);
    new_ldata->b = st_init(orig_ldata->b);
    new_ldata->c = st_init(orig_ldata->c);
    new_ldata->d = st_init(orig_ldata->d);

    *ldata_p = new_ldata;
}

void my_leaf_symb_destroy(uint64_t ldata)
{
    lsymb_t *data_p = (lsymb_t*) ldata; // Data in leaf = pointer to my data
    st_delete(data_p->a);
    st_delete(data_p->b);
    st_delete(data_p->c);
    st_delete(data_p->d);
    free(data_p);
}

int my_leaf_symb_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    lsymb_t *ldata_a = (lsymb_t *) ldata_a_raw;
    lsymb_t *ldata_b = (lsymb_t *) ldata_b_raw;

    return !st_cmp(ldata_a->a, ldata_b->a) && !st_cmp(ldata_a->b, ldata_b->b) && !st_cmp(ldata_a->c, ldata_b->c) \
           && !st_cmp(ldata_a->d, ldata_b->d);
}

uint64_t my_leaf_symb_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    (void)seed; // not needed when using leaves of a single type
    lsymb_t *ldata = (lsymb_t*) ldata_raw;

    uint64_t val = 0;
    val = MY_HASH_COMB_PTR(val, ldata->a);
    val = MY_HASH_COMB_PTR(val, ldata->b);
    val = MY_HASH_COMB_PTR(val, ldata->c);
    val = MY_HASH_COMB_PTR(val, ldata->d);
    val = MY_HASH_COMB_PTR(val, cs_k);

    return val;
}

/* CUSTOM MTBDD OPERATIONS */
//TODO: should be next_var one global variable? maybe better as param of the function
TASK_IMPL_3(MTBDD, mtbdd_to_symb, MTBDD, a, coef_t*, map, vars_t*, next_var)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(a)) {
        cnum *orig_data = (cnum*) mtbdd_getvalue(a);
        mpz_init_set(map[*next_var], orig_data->a);
        mpz_init_set(map[*next_var + 1], orig_data->b);
        mpz_init_set(map[*next_var + 2], orig_data->c);
        mpz_init_set(map[*next_var + 3], orig_data->d);

        lsymb_t *new_data = my_malloc(sizeof(lsymb_t)); //TODO: should be malloc? (check where is the free)
        new_data->a = st_create(*next_var);
        new_data->b = st_create(*next_var + 1);
        new_data->c = st_create(*next_var + 2);
        new_data->d = st_create(*next_var + 3);

        new_data->var_a = *next_var;
        new_data->var_b = *next_var + 1;
        new_data->var_c = *next_var + 2;
        new_data->var_d = *next_var + 3;

        MTBDD res = mtbdd_makeleaf(ltype_s_id, (uint64_t) new_data);
        *next_var += 4;

        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Evaluates the given expression according to the map
 */
static coef_t* eval_var(stree_t *data,  coef_t* map)
{
    coef_t *res = malloc(sizeof(mpz_t));
    mpz_init(*res);

    if (data->type == ST_VAL) {
        mpz_set(*res, map[data->val->var]);
        mpz_mul(*res, *res, data->val->coef);
    }
    else {
        coef_t *l = eval_var(data->ls, map);
        coef_t *r = eval_var(data->rs, map);
        data->type == ST_ADD? mpz_add(*res, *l, *r) : mpz_mul(*res, *l, *r);
    }

    return res;
}

TASK_IMPL_3(MTBDD, mtbdd_update_map, MTBDD, a, coef_t*, map, coef_t*, new_map) // TODO: should be an MTBDD task?
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(a)) {
        lsymb_t *data = (lsymb_t*) mtbdd_getvalue(a);

        coef_t *res_a = eval_var(data->a, map);
        coef_t *res_b = eval_var(data->b, map);
        coef_t *res_c = eval_var(data->c, map);
        coef_t *res_d = eval_var(data->d, map);

        mpz_init_set(new_map[data->var_a], *res_a);
        mpz_init_set(new_map[data->var_b], *res_b);
        mpz_init_set(new_map[data->var_c], *res_c);
        mpz_init_set(new_map[data->var_d], *res_d);

        mpz_clears(*res_a, *res_b, *res_c, *res_d, NULL);
        free(res_a);
        free(res_b);
        free(res_c);
        free(res_d);

        return a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_from_symb, MTBDD, a, coef_t*, map)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(a)) {
        lsymb_t *data = (lsymb_t*) mtbdd_getvalue(a);
        
        cnum new_data; // can be local, mtbdd_makeleaf makes realloc
        mpz_init_set(new_data.a, map[data->var_a]);
        mpz_init_set(new_data.b, map[data->var_b]);
        mpz_init_set(new_data.c, map[data->var_c]);
        mpz_init_set(new_data.d, map[data->var_d]);
        
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &new_data);
        mpz_clears(new_data.a, new_data.b, new_data.c, new_data.d, NULL);

        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

/* end of "custom_mtbdd_symb.c" */
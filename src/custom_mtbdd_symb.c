#include "custom_mtbdd_symb.h"

/*
 * Custom leaf implementation is taken from: https://github.com/MichalHe/sylvan-custom-leaf-example
 */

uint32_t ltype_s_id;
extern vars_t next_var;

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
TASK_IMPL_2(MTBDD, mtbdd_to_symb, MTBDD, a, uint64_t*, arr)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(a)) {
        cnum *orig_data = (cnum*) mtbdd_getvalue(a);
        arr[next_var] = (uint64_t) orig_data->a; // needs typecasting as mpz_t is an array type
        arr[next_var + 1] = (uint64_t) orig_data->b;
        arr[next_var + 2] = (uint64_t) orig_data->c;
        arr[next_var + 3] = (uint64_t) orig_data->d;

        lsymb_t *new_data = my_malloc(sizeof(lsymb_t)); //TODO: check where is free
        new_data->a = st_create(next_var);
        new_data->b = st_create(next_var + 1);
        new_data->c = st_create(next_var + 2);
        new_data->d = st_create(next_var + 3);

        MTBDD res = mtbdd_makeleaf(ltype_s_id, (uint64_t) new_data);
        next_var += 4;

        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

/* end of "custom_mtbdd_symb.c" */
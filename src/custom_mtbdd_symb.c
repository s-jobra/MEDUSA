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
    sylvan_mt_set_hash(ltype_s_id, my_leaf_hash);
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

uint64_t my_leaf_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    (void)seed; // not needed when using leaves of a single type
    lsymb_t *ldata = (lsymb_t*) ldata_raw;

    uint64_t val = 0;
    val = MY_HASH_COMB(val, ldata->a);
    val = MY_HASH_COMB(val, ldata->b);
    val = MY_HASH_COMB(val, ldata->c);
    val = MY_HASH_COMB(val, ldata->d);
    val = MY_HASH_COMB(val, cs_k);

    return val;
}

/* end of "custom_mtbdd_symb.c" */
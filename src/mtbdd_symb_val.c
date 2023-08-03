#include "mtbdd_symb_val.h"


uint32_t ltype_symb_expr_id;  // leaf type id for symbolic representation

coefs_k_t cs_k; // coefficient k common for all MTBDD leaf values for symbolic representation


/**
 * Max. size of string written as leaf value in output file.
 */
#define MAX_SYMB_LEAF_STR_LEN MAX_ST_TO_STR_LEN * 5


/* SETUP */
void init_my_leaf_symb_val()
{
    ltype_symb_expr_id = sylvan_mt_create_type();

    sylvan_mt_set_create(ltype_symb_expr_id, my_leaf_symb_v_create);
    sylvan_mt_set_destroy(ltype_symb_expr_id, my_leaf_symb_v_destroy);
    sylvan_mt_set_equals(ltype_symb_expr_id, my_leaf_symb_v_equals);
    sylvan_mt_set_to_str(ltype_symb_expr_id, my_leaf_symb_v_to_str);
    sylvan_mt_set_hash(ltype_symb_expr_id, my_leaf_symb_v_hash);
}

/* CUSTOM HANDLES */
void my_leaf_symb_v_create(uint64_t *ldata_p_raw)
{
    sl_val_t** ldata_p = (sl_val_t**)ldata_p_raw; // Leaf data type is uint64_t, we store there ptr to our actual data
    
    sl_val_t* orig_ldata = *ldata_p;
    sl_val_t* new_ldata = (sl_val_t*)my_malloc(sizeof(sl_val_t));

    new_ldata->a = st_init(orig_ldata->a);
    new_ldata->b = st_init(orig_ldata->b);
    new_ldata->c = st_init(orig_ldata->c);
    new_ldata->d = st_init(orig_ldata->d);

    *ldata_p = new_ldata;
}

void my_leaf_symb_v_destroy(uint64_t ldata)
{
    sl_val_t *data_p = (sl_val_t*) ldata; // Data in leaf = pointer to my data
    st_delete(data_p->a);
    st_delete(data_p->b);
    st_delete(data_p->c);
    st_delete(data_p->d);
    free(data_p);
}

int my_leaf_symb_v_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    sl_val_t *ldata_a = (sl_val_t *) ldata_a_raw;
    sl_val_t *ldata_b = (sl_val_t *) ldata_b_raw;

    return !st_cmp(ldata_a->a, ldata_b->a) && !st_cmp(ldata_a->b, ldata_b->b) && !st_cmp(ldata_a->c, ldata_b->c) \
           && !st_cmp(ldata_a->d, ldata_b->d);
}

char* my_leaf_symb_v_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize)
{
    (void) complemented;
    sl_val_t *ldata = (sl_val_t*) ldata_raw;

    char ldata_string[MAX_SYMB_LEAF_STR_LEN] = {0};
    
    int chars_written = gmp_snprintf(ldata_string, MAX_SYMB_LEAF_STR_LEN, "(1/√2)^(%Zd) * (%s + %sω +%sω² + %sω³)", \
                                     cs_k, st_to_str(ldata->a), st_to_str(ldata->b), st_to_str(ldata->c), st_to_str(ldata->d));
    // Was string truncated?
    if (chars_written >= MAX_SYMB_LEAF_STR_LEN) {
        error_exit("Allocated string length for leaf value output has not been sufficient.\n");
    }
    else if (chars_written < 0) {
        error_exit("An encoding error has occured when producing leaf value output.\n");
    }

    // Is buffer large enough?
    if (chars_written < sylvan_bufsize) {
        memcpy(sylvan_buf, ldata_string, chars_written * sizeof(char));
        sylvan_buf[chars_written] = '\0';
        return sylvan_buf;
    }
    
    // Else return newly allocated string
    char *new_buf = (char*)my_malloc((chars_written + 1) * sizeof(char));
    memcpy(new_buf, ldata_string, chars_written * sizeof(char));
    new_buf[chars_written] = '\0';
    return new_buf;
}

uint64_t my_leaf_symb_v_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    sl_val_t *ldata = (sl_val_t*) ldata_raw;

    uint64_t val = seed;
    val = MY_HASH_COMB_SYMB(val, ldata->a);
    val = MY_HASH_COMB_SYMB(val, ldata->b);
    val = MY_HASH_COMB_SYMB(val, ldata->c);
    val = MY_HASH_COMB_SYMB(val, ldata->d);
    //FIXME: check performance - needed? val = MY_HASH_COMB_SYMB(val, cs_k);

    return val;
}

/* CUSTOM MTBDD OPERATIONS */
TASK_IMPL_2(MTBDD, mtbdd_map_to_symb_val, MTBDD, t, size_t, x)
{
    (void) x; // needed for TASK_IMPL_2

    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(t)) {
        sl_map_t *t_data = (sl_map_t*) mtbdd_getvalue(t);
        sl_val_t new_data;
        
        new_data.a = st_create_val(t_data->va);
        new_data.b = st_create_val(t_data->vb);
        new_data.c = st_create_val(t_data->vc);
        new_data.d = st_create_val(t_data->vd);

        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &new_data);

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
        if (data->val->coef != 0) {
            mpz_set(*res, map[data->val->var]);
            mpz_mul(*res, *res, data->val->coef);
        }
    }
    else {
        coef_t *l = eval_var(data->ls, map);
        coef_t *r = eval_var(data->rs, map);
        if (data->type == ST_ADD) {
            mpz_add(*res, *l, *r);
        }
        else if (data->type == ST_SUB) {
            mpz_sub(*res, *l, *r);
        }
        else {
            mpz_mul(*res, *l, *r);
        }
    }

    return res;
}

#include <sylvan_int.h>

VOID_TASK_IMPL_4(mtbdd_update_map, MTBDD, mtbdd_map, MTBDD, mtbdd_val, coef_t*, map, coef_t*, new_map)
{
    //TODO: gc + cache?

    if (mtbdd_val == mtbdd_false) {
        sl_map_t *map_data = (sl_map_t*) mtbdd_getvalue(mtbdd_map);
        mpz_inits(new_map[map_data->va], new_map[map_data->vb], new_map[map_data->vc], new_map[map_data->vd], NULL);
        return;
    }

    int lmap = mtbdd_isleaf(mtbdd_map);
    int lval = mtbdd_isleaf(mtbdd_val);
    if (lmap && lval) {
        sl_map_t *mdata = (sl_map_t*) mtbdd_getvalue(mtbdd_map);
        sl_val_t *vdata = (sl_val_t*) mtbdd_getvalue(mtbdd_val);

        coef_t *res_a = eval_var(vdata->a, map);
        coef_t *res_b = eval_var(vdata->b, map);
        coef_t *res_c = eval_var(vdata->c, map);
        coef_t *res_d = eval_var(vdata->d, map);

        mpz_set(new_map[mdata->va], *res_a);
        mpz_set(new_map[mdata->vb], *res_b);
        mpz_set(new_map[mdata->vc], *res_c);
        mpz_set(new_map[mdata->vd], *res_d);

        mpz_clears(*res_a, *res_b, *res_c, *res_d, NULL);
        free(res_a);
        free(res_b);
        free(res_c);
        free(res_d);
    }
    else {
        /* Get top variable */
        mtbddnode_t n_map, n_val;
        uint32_t v_map, v_val;
        if (!lmap) {
            n_map = MTBDD_GETNODE(mtbdd_map);
            v_map = mtbddnode_getvariable(n_map);
        }
        else {
            n_map = 0;
            v_map = 0xffffffff;
        }

        if (!lval) {
            n_val = MTBDD_GETNODE(mtbdd_val);
            v_val = mtbddnode_getvariable(n_val);
        }
        else {
            n_val = 0;
            v_val = 0xffffffff;
        }
        uint32_t v = v_map < v_val ? v_map : v_val;

        /* Get cofactors */
        MTBDD map_low, map_high, val_low, val_high;
        if (!lmap && v_map == v) {
            map_low = node_getlow(mtbdd_map, n_map);
            map_high = node_gethigh(mtbdd_map, n_map);
        }
        else {
            map_low = mtbdd_map;
            map_high = mtbdd_map;
        }

        if (!lval && v_val == v) {
            val_low = node_getlow(mtbdd_val, n_val);
            val_high = node_gethigh(mtbdd_val, n_val);
        }
        else {
            val_low = mtbdd_val;
            val_high = mtbdd_val;
        }

        /* Recursive */
        SPAWN(mtbdd_update_map, map_high, val_high, map, new_map);
        CALL(mtbdd_update_map, map_low, val_low, map, new_map);
        SYNC(mtbdd_update_map);
    }
}

TASK_IMPL_2(MTBDD, mtbdd_from_symb, MTBDD, t, size_t, raw_map)
{
    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(t)) {
        coef_t* map = (coef_t*) raw_map;
        sl_map_t *data = (sl_map_t*) mtbdd_getvalue(t);
        
        cnum new_data; // can be local, mtbdd_makeleaf makes realloc
        mpz_init_set(new_data.a, map[data->va]);
        mpz_init_set(new_data.b, map[data->vb]);
        mpz_init_set(new_data.c, map[data->vc]);
        mpz_init_set(new_data.d, map[data->vd]);

        if (!mpz_cmp_si(new_data.a, 0) && !mpz_cmp_si(new_data.b, 0) && !mpz_cmp_si(new_data.c, 0) && !mpz_cmp_si(new_data.d, 0)) {
            mpz_clears(new_data.a, new_data.b, new_data.c, new_data.d, NULL);
            return (MTBDD)NULL;
        }
        
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &new_data);
        mpz_clears(new_data.a, new_data.b, new_data.c, new_data.d, NULL);

        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

// Operations needed for gate representation:
TASK_IMPL_2(MTBDD, mtbdd_symb_plus, MTBDD*, p_a, MTBDD*, p_b)
{
    MTBDD a = *p_a;
    MTBDD b = *p_b;

    // Partial function check
    if (a == mtbdd_false) return b;
    if (b == mtbdd_false) return a;

    // Compute a + b if both mtbdds are leaves
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        sl_val_t *a_data = (sl_val_t*) mtbdd_getvalue(a);
        sl_val_t *b_data = (sl_val_t*) mtbdd_getvalue(b);

        sl_val_t res_data;
        res_data.a = st_op(a_data->a, b_data->a, ST_ADD);
        res_data.b = st_op(a_data->b, b_data->b, ST_ADD);
        res_data.c = st_op(a_data->c, b_data->c, ST_ADD);
        res_data.d = st_op(a_data->d, b_data->d, ST_ADD);
        
        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &res_data);
        return res;
    }

    /* Commutative, so swap a,b for better cache performance*/
    if (a < b) {
        *p_a = b;
        *p_b = a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_minus, MTBDD*, p_a, MTBDD*, p_b)
{
    MTBDD a = *p_a;
    MTBDD b = *p_b;

    // Partial function check
    if (a == mtbdd_false){
        MTBDD b_minus = my_mtbdd_symb_neg(b);
        return b_minus;
    } 
    if (b == mtbdd_false) return a;

    // Compute a - b if both mtbdds are leaves
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        sl_val_t *a_data = (sl_val_t*) mtbdd_getvalue(a);
        sl_val_t *b_data = (sl_val_t*) mtbdd_getvalue(b);
        
        sl_val_t res_data;
        res_data.a = st_op(a_data->a, b_data->a, ST_SUB);
        res_data.b = st_op(a_data->b, b_data->b, ST_SUB);
        res_data.c = st_op(a_data->c, b_data->c, ST_SUB);
        res_data.d = st_op(a_data->d, b_data->d, ST_SUB);
        
        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_neg, MTBDD, t, size_t, x)
{
    (void)x; // extra parameter needed for task - not needed

    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    // Compute -a if mtbdd is a leaf
    if (mtbdd_isleaf(t)) {
        sl_val_t *ldata = (sl_val_t*) mtbdd_getvalue(t);

        sl_val_t res_data;
        res_data.a = st_init(ldata->a);
        res_data.b = st_init(ldata->b);
        res_data.c = st_init(ldata->c);
        res_data.d = st_init(ldata->d);
        st_coef_mul(res_data.a, -1);
        st_coef_mul(res_data.b, -1);
        st_coef_mul(res_data.c, -1);
        st_coef_mul(res_data.d, -1);

        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_b_xt_mul, MTBDD*, p_t, MTBDD*, p_b)
{
    MTBDD t = *p_t;
    MTBDD b = *p_b;

    // Partial function check
    if (b == mtbdd_true) return t;
    if (b == mtbdd_false || t == mtbdd_false) return mtbdd_false;

    return mtbdd_invalid; // Recurse deeper
}

MTBDD mtbdd_symb_b_xt_mul_wrapper(MTBDD t, uint32_t xt)
{
    MTBDD b_xt = sylvan_ithvar(xt);

    return mtbdd_apply(t, b_xt, TASK(mtbdd_symb_b_xt_mul));
}


MTBDD mtbdd_symb_b_xt_comp_mul_wrapper(MTBDD t, uint32_t xt)
{
    MTBDD b_xt_comp = sylvan_nithvar(xt);

    return mtbdd_apply(t, b_xt_comp, TASK(mtbdd_symb_b_xt_mul));
}

/* end of "mtbdd_symb_val.c" */
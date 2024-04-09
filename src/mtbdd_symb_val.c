#include "mtbdd_symb_val.h"


uint32_t ltype_symb_expr_id;  // leaf type id for symbolic representation

coefs_k_t cs_k; // coefficient k common for all MTBDD leaf values for symbolic representation


/**
 * Max. size of string written as leaf value in output file.
 */
#define MAX_SYMB_LEAF_STR_LEN MAX_ST_TO_STR_LEN * 5


void cs_k_reset()
{
    mpz_set_ui(cs_k, 0);
}

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

    new_ldata->a = orig_ldata->a;
    new_ldata->b = orig_ldata->b;
    new_ldata->c = orig_ldata->c;
    new_ldata->d = orig_ldata->d;

    *ldata_p = new_ldata;
}

void my_leaf_symb_v_destroy(uint64_t ldata)
{
    sl_val_t *data_p = (sl_val_t*) ldata; // Data in leaf = pointer to my data
    // Called only after htab_clear() -> when leaf data has already been deallocated
    free(data_p);
}

int my_leaf_symb_v_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    sl_val_t *ldata_a = (sl_val_t *) ldata_a_raw;
    sl_val_t *ldata_b = (sl_val_t *) ldata_b_raw;

    return symexp_cmp(ldata_a->a, ldata_b->a) && symexp_cmp(ldata_a->b, ldata_b->b) && symexp_cmp(ldata_a->c, ldata_b->c) \
           && symexp_cmp(ldata_a->d, ldata_b->d);
}

char* my_leaf_symb_v_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize)
{
    (void) complemented;
    sl_val_t *ldata = (sl_val_t*) ldata_raw;

    char ldata_string[MAX_SYMB_LEAF_STR_LEN] = {0};
    
    int chars_written = gmp_snprintf(ldata_string, MAX_SYMB_LEAF_STR_LEN, "(1/√2)^(%Zd) * ((%s) + (%s)ω + (%s)ω² + (%s)ω³)", \
                                     cs_k, symexp_to_str(ldata->a), symexp_to_str(ldata->b), symexp_to_str(ldata->c), symexp_to_str(ldata->d));
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
    val = MY_HASH_COMB(val, ldata->a);
    val = MY_HASH_COMB(val, ldata->b);
    val = MY_HASH_COMB(val, ldata->c);
    val = MY_HASH_COMB(val, ldata->d);

    return val;
}

/* CUSTOM MTBDD OPERATIONS */
TASK_IMPL_2(MTBDD, mtbdd_map_to_symb_val, MTBDD, t, size_t, raw_map)
{
    // Partial function check not needed, 4 variables were assigned to every base vector

    if (mtbdd_isleaf(t)) {
        coef_t* map = (coef_t*) raw_map;
        sl_map_t *t_data = (sl_map_t*) mtbdd_getvalue(t);
        sl_val_t new_data;

        if (!mpz_cmp_si(map[t_data->va], 0) && !mpz_cmp_si(map[t_data->vb], 0) &&
            !mpz_cmp_si(map[t_data->vc], 0) && !mpz_cmp_si(map[t_data->vd], 0)) {
            return mtbdd_false;
        }

        new_data.a = symexp_init(t_data->va);
        new_data.b = symexp_init(t_data->vb);
        new_data.c = symexp_init(t_data->vc);
        new_data.d = symexp_init(t_data->vd);
        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &new_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Evaluates the given expression according to the map
 */
static coef_t* eval_var(symexp_list_t *expr,  coef_t* map)
{
    coef_t *res = malloc(sizeof(mpz_t)); // mpz_t is an array type (can't return from function)
    coef_t imm_res;
    mpz_inits(*res, imm_res, NULL);

    if (expr != NULL) {
        symexp_list_first(expr);
        while(expr->active) {
            mpz_set(imm_res, map[expr->active->data->var]);
            mpz_mul(imm_res, imm_res, expr->active->data->coef);
            mpz_add(*res, *res, imm_res);
            symexp_list_next(expr);
        }
    }

    mpz_clear(imm_res);
    return res;
}

VOID_TASK_IMPL_4(mtbdd_update_map, MTBDD, mtbdd_map, MTBDD, mtbdd_val, coef_t*, map, coef_t*, new_map)
{
    //TODO: gc + cache?

    int lmap = mtbdd_isleaf(mtbdd_map);
    int lval = mtbdd_isleaf(mtbdd_val);
    if (lmap && lval) {
        if (mtbdd_val == mtbdd_false) {
            sl_map_t *map_data = (sl_map_t*) mtbdd_getvalue(mtbdd_map);
            mpz_set_ui(new_map[map_data->va], 0);
            mpz_set_ui(new_map[map_data->vb], 0);
            mpz_set_ui(new_map[map_data->vc], 0);
            mpz_set_ui(new_map[map_data->vd], 0);
            return;
        }

        sl_map_t *mdata = (sl_map_t*) mtbdd_getvalue(mtbdd_map);
        sl_val_t *vdata = (sl_val_t*) mtbdd_getvalue(mtbdd_val);

        //TODO: clear new_map beforehand and only eval_var() if not 0?
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
            return mtbdd_false;
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
        res_data.a = symexp_op(a_data->a, b_data->a, SYMEXP_ADD);
        res_data.b = symexp_op(a_data->b, b_data->b, SYMEXP_ADD);
        res_data.c = symexp_op(a_data->c, b_data->c, SYMEXP_ADD);
        res_data.d = symexp_op(a_data->d, b_data->d, SYMEXP_ADD);

        if (!res_data.a && !res_data.b && !res_data.c && !res_data.d) {
            return mtbdd_false;
        }
        
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
        res_data.a = symexp_op(a_data->a, b_data->a, SYMEXP_SUB);
        res_data.b = symexp_op(a_data->b, b_data->b, SYMEXP_SUB);
        res_data.c = symexp_op(a_data->c, b_data->c, SYMEXP_SUB);
        res_data.d = symexp_op(a_data->d, b_data->d, SYMEXP_SUB);

        if (!res_data.a && !res_data.b && !res_data.c && !res_data.d) {
            return mtbdd_false;
        }
        
        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_times_c, MTBDD, t, size_t, c_raw)
{
    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    // Compute c*t if mtbdd is a leaf
    if (mtbdd_isleaf(t)) {
        sl_val_t *ldata = (sl_val_t*) mtbdd_getvalue(t);
        unsigned long c = (unsigned long)c_raw;

        sl_val_t res_data;
        res_data.a = symexp_mul_c(ldata->a, c);
        res_data.b = symexp_mul_c(ldata->b,c);
        res_data.c = symexp_mul_c(ldata->c, c);
        res_data.d = symexp_mul_c(ldata->d, c);

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
        res_data.a = symexp_op(NULL, ldata->a, SYMEXP_SUB);
        res_data.b = symexp_op(NULL, ldata->b, SYMEXP_SUB);
        res_data.c = symexp_op(NULL, ldata->c, SYMEXP_SUB);
        res_data.d = symexp_op(NULL, ldata->d, SYMEXP_SUB);

        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_coef_rot1, MTBDD, t, size_t, x)
{
    (void)x; // extra parameter needed for task - not needed

    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    // Compute coeficient rotation if mtbdd is a leaf
    if (mtbdd_isleaf(t)) {
        sl_val_t *ldata = (sl_val_t*) mtbdd_getvalue(t);

        sl_val_t res_data;
        res_data.a = symexp_op(NULL, ldata->d, SYMEXP_SUB);
        res_data.b = ldata->a;
        res_data.c = ldata->b;
        res_data.d = ldata->c;

        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_coef_rot2, MTBDD, t, size_t, x)
{
    (void)x; // extra parameter needed for task - not needed

    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    // Compute coeficient rotation if mtbdd is a leaf
    if (mtbdd_isleaf(t)) {
        sl_val_t *ldata = (sl_val_t*) mtbdd_getvalue(t);

        sl_val_t res_data;
        res_data.a = symexp_op(NULL, ldata->c, SYMEXP_SUB);
        res_data.b = symexp_op(NULL, ldata->d, SYMEXP_SUB);
        res_data.c = ldata->a;
        res_data.d = ldata->b;

        MTBDD res = mtbdd_makeleaf(ltype_symb_expr_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

/* end of "mtbdd_symb_val.c" */
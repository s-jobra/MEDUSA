#include "custom_mtbdd_symb.h"

/*
 * Custom leaf implementation is taken from: https://github.com/MichalHe/sylvan-custom-leaf-example
 */

uint32_t ltype_s_id;

/**
 * Max. size of string written as leaf value in output file.
 */
#define MAX_SYMB_LEAF_STR_LEN 10000

/* SETUP */
void init_my_leaf_symb()
{
    ltype_s_id = sylvan_mt_create_type();

    sylvan_mt_set_create(ltype_s_id, my_leaf_symb_create);
    sylvan_mt_set_destroy(ltype_s_id, my_leaf_symb_destroy);
    sylvan_mt_set_equals(ltype_s_id, my_leaf_symb_equals);
    sylvan_mt_set_to_str(ltype_s_id, my_leaf_symb_to_str);
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

char* my_leaf_symb_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize)
{
    (void) complemented;
    lsymb_t *ldata = (lsymb_t*) ldata_raw;

    char ldata_string[MAX_SYMB_LEAF_STR_LEN ] = {0};
    
    //TODO: print the whole expression (tree)?
    int chars_written = gmp_snprintf(ldata_string, MAX_SYMB_LEAF_STR_LEN , "(1/√2)^(%Zd) * (%ld + %ldω +%ldω² + %ldω³)", \
                                     cs_k, ldata->var_a, ldata->var_b, ldata->var_c, ldata->var_d);
    // Was string truncated?
    if (chars_written >= MAX_SYMB_LEAF_STR_LEN ) {
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

uint64_t my_leaf_symb_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    lsymb_t *ldata = (lsymb_t*) ldata_raw;

    uint64_t val = seed;
    val = MY_HASH_COMB_PTR(val, ldata->a);
    val = MY_HASH_COMB_PTR(val, ldata->b);
    val = MY_HASH_COMB_PTR(val, ldata->c);
    val = MY_HASH_COMB_PTR(val, ldata->d);
    val = MY_HASH_COMB_PTR(val, cs_k);

    return val;
}

/* CUSTOM MTBDD OPERATIONS */
//TODO: should be next_var one global variable? maybe better as just a param of the function
TASK_IMPL_2(MTBDD, mtbdd_to_symb, MTBDD, a, size_t, raw_m)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(a)) {
        vmap_t* m = (vmap_t*) raw_m;

        cnum *orig_data = (cnum*) mtbdd_getvalue(a);
        mpz_init_set(m->map[m->next_var], orig_data->a);
        mpz_init_set(m->map[m->next_var + 1], orig_data->b);
        mpz_init_set(m->map[m->next_var + 2], orig_data->c);
        mpz_init_set(m->map[m->next_var + 3], orig_data->d);

        lsymb_t *new_data = my_malloc(sizeof(lsymb_t)); //TODO: should be malloc? (check where is the free)
        new_data->a = st_create(m->next_var);
        new_data->b = st_create(m->next_var + 1);
        new_data->c = st_create(m->next_var + 2);
        new_data->d = st_create(m->next_var + 3);

        //TODO:FIXME: all variables are now 0
        new_data->var_a = m->next_var;
        new_data->var_b = m->next_var + 1;
        new_data->var_c = m->next_var + 2;
        new_data->var_d = m->next_var + 3;

        MTBDD res = mtbdd_makeleaf(ltype_s_id, (uint64_t) new_data);
        m->next_var += 4;

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
        //TODO: first check if coef is not 0
        mpz_set(*res, map[data->val->var]);
        mpz_mul(*res, *res, data->val->coef);
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

TASK_IMPL_2(MTBDD, mtbdd_from_symb, MTBDD, a, size_t, raw_map)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(a)) {
        coef_t* map = (coef_t*) raw_map;
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
        lsymb_t *a_data = (lsymb_t*) mtbdd_getvalue(a);
        lsymb_t *b_data = (lsymb_t*) mtbdd_getvalue(b);
        
        //FIXME: assert a,b,c,d in both data are the same var???

        lsymb_t res_data;
        res_data.var_a = a_data->var_a;
        res_data.var_b = a_data->var_b;
        res_data.var_c = a_data->var_c;
        res_data.var_d = a_data->var_d;
        res_data.a = st_op(a_data->a, b_data->a, ST_ADD);
        res_data.b = st_op(a_data->b, b_data->b, ST_ADD);
        res_data.c = st_op(a_data->c, b_data->c, ST_ADD);
        res_data.d = st_op(a_data->d, b_data->d, ST_ADD);
        
        MTBDD res = mtbdd_makeleaf(ltype_s_id, (uint64_t) &res_data);
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
        lsymb_t *a_data = (lsymb_t*) mtbdd_getvalue(a);
        lsymb_t *b_data = (lsymb_t*) mtbdd_getvalue(b);
        
        //FIXME: assert a,b,c,d in both data are the same var???

        lsymb_t res_data;
        res_data.var_a = a_data->var_a;
        res_data.var_b = a_data->var_b;
        res_data.var_c = a_data->var_c;
        res_data.var_d = a_data->var_d;
        res_data.a = st_op(a_data->a, b_data->a, ST_SUB);
        res_data.b = st_op(a_data->b, b_data->b, ST_SUB);
        res_data.c = st_op(a_data->c, b_data->c, ST_SUB);
        res_data.d = st_op(a_data->d, b_data->d, ST_SUB);
        
        MTBDD res = mtbdd_makeleaf(ltype_s_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_neg, MTBDD, a, size_t, x)
{
    (void)x; // extra parameter needed for task - not needed

    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Compute -a if mtbdd is a leaf
    if (mtbdd_isleaf(a)) {
        lsymb_t *a_data = (lsymb_t*) mtbdd_getvalue(a);

        lsymb_t res_data;
        res_data.var_a = a_data->var_a;
        res_data.var_b = a_data->var_b;
        res_data.var_c = a_data->var_c;
        res_data.var_d = a_data->var_d;
        res_data.a = st_init(a_data->a); //FIXME: maybe can just copy pointers??
        res_data.b = st_init(a_data->b);
        res_data.c = st_init(a_data->c);
        res_data.d = st_init(a_data->d);
        st_coef_mul(a_data->a, -1);
        st_coef_mul(a_data->b, -1);
        st_coef_mul(a_data->c, -1);
        st_coef_mul(a_data->d, -1);

        MTBDD res = mtbdd_makeleaf(ltype_s_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_b_xt_mul, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // If xt, ground the low edge
    if (mtbdd_isnode(a)) {
        if (mtbdd_getvar(a) == (uint32_t)xt) { // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
            MTBDD res = mtbdd_makenode(xt, mtbdd_false, mtbdd_gethigh(a));
            return res;
        }
    }
    // Else copy if mtbdd is leaf
    else {
        return a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, mtbdd_symb_b_xt_comp_mul, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // If xt, ground the high edge
    if (mtbdd_isnode(a)) {
        if (mtbdd_getvar(a) == (uint32_t)xt) { // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
            MTBDD res = mtbdd_makenode(xt, mtbdd_getlow(a), mtbdd_false);
            return res;
        }
    }
    // Else copy if mtbdd is leaf
    else {
        return a;
    }

    return mtbdd_invalid; // Recurse deeper
}

/* end of "custom_mtbdd_symb.c" */
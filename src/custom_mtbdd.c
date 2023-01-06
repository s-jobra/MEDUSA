#include "custom_mtbdd.h"

/**
 * Max. size of string written as leaf value in output file.
 */
#define MAX_LEAF_STR_LEN 32000

uint32_t ltype_id;

/* INTERNAL */
/**
 * Hash value and combine it with an already existing hash
 * 
 * Taken from: http://www.boost.org/doc/libs/1_64_0/boost/functional/hash/hash.hpp
 */
#define MY_HASH_COMB(val, data) ( (val) ^ (my_hash((uint64_t)(data)) + 0x9e3779b9 + ((val)<<6) + ((val)>>2)) )

/**
 * Hash function for 64bit integers.
 * 
 * Taken from: https://stackoverflow.com/a/12996028, author: Thomas Mueller
 */
static inline uint64_t my_hash(uint64_t x) {
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);
    return x;
}

/**
 * Custom malloc function including error handling.
 */
static void* my_malloc(size_t size) {
    void* p = malloc(size);
    if (p == NULL) {
        error_exit("Bad memory allocation.");
    }

    return p;
}

/* SETUP */
void init_sylvan() {
    lace_start(2, 0); // 1 thread, default task queue size
    sylvan_set_limits(500LL*1024*1024, 3, 5); // Allocate 500MB
    sylvan_init_package();
    sylvan_init_mtbdd();
}

void init_my_leaf()
{
    ltype_id = sylvan_mt_create_type();

    sylvan_mt_set_create(ltype_id, my_leaf_create);
    sylvan_mt_set_destroy(ltype_id, my_leaf_destroy);
    sylvan_mt_set_equals(ltype_id, my_leaf_equals);
    sylvan_mt_set_to_str(ltype_id, my_leaf_to_str);
    sylvan_mt_set_hash(ltype_id, my_leaf_hash);
}

/* CUSTOM HANDLES */
void my_leaf_create(uint64_t* ldata_p_raw)
{
    cnum** ldata_p = (cnum**)ldata_p_raw; // Leaf data type is uint64_t, we store there ptr to our actual data
    
    cnum* orig_ldata = *ldata_p;
    cnum* new_ldata = (cnum*)my_malloc(sizeof(cnum));

    mpz_init_set(new_ldata->a, orig_ldata->a);
    mpz_init_set(new_ldata->b, orig_ldata->b);
    mpz_init_set(new_ldata->c, orig_ldata->c);
    mpz_init_set(new_ldata->d, orig_ldata->d);
    mpz_init_set(new_ldata->k, orig_ldata->k);

    *ldata_p = new_ldata;
}

void my_leaf_destroy(uint64_t ldata)
{
    cnum* data_p = (cnum*) ldata; // Data in leaf = pointer to my data
    mpz_clears(data_p->a, data_p->b, data_p->c, data_p->d, data_p->k, NULL);
    free(data_p);
}

int my_leaf_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    cnum* ldata_a = (cnum*) ldata_a_raw;
    cnum* ldata_b = (cnum*) ldata_b_raw;

    return !mpz_cmp(ldata_a->a, ldata_b->a) && !mpz_cmp(ldata_a->b, ldata_b->b) && !mpz_cmp(ldata_a->c, ldata_b->c) \
           && !mpz_cmp(ldata_a->d, ldata_b->d) && !mpz_cmp(ldata_a->k, ldata_b->k);
}

char* my_leaf_to_str(int complemented, uint64_t ldata_raw, char* sylvan_buf, size_t sylvan_bufsize)
{
    (void) complemented;
    cnum* ldata = (cnum*) ldata_raw;

    char ldata_string[MAX_LEAF_STR_LEN] = {0};
    
    int chars_written = gmp_snprintf(ldata_string, MAX_LEAF_STR_LEN, "(1/√2)^(%Zd) * (%Zd%+Zdω%+Zdω²%+Zdω³)", ldata->k, ldata->a, ldata->b, ldata->c, ldata->d);
    // Was string truncated?
    if (chars_written >= MAX_LEAF_STR_LEN) {
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
    char* new_buf = (char*)my_malloc((chars_written + 1) * sizeof(char));
    memcpy(new_buf, ldata_string, chars_written * sizeof(char));
    new_buf[chars_written] = '\0';
    return new_buf;
}

uint64_t my_leaf_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    (void)seed; // not needed when using leaves of a single type
    cnum* ldata = (cnum*) ldata_raw;

    uint64_t val = 0;
    val = MY_HASH_COMB(val, ldata->a);
    val = MY_HASH_COMB(val, ldata->b);
    val = MY_HASH_COMB(val, ldata->c);
    val = MY_HASH_COMB(val, ldata->d);
    val = MY_HASH_COMB(val, ldata->k);

    return val;
}

/* CUSTOM MTBDD OPERATIONS */
// Basic operations:
TASK_IMPL_2(MTBDD, my_op_plus, MTBDD*, p_a, MTBDD*, p_b)
{
    MTBDD a = *p_a;
    MTBDD b = *p_b;

    // Partial function check
    if (a == mtbdd_false) return b;
    if (b == mtbdd_false) return a;

    // Compute a + b if both mtbdds are leaves
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);
        cnum* b_data = (cnum*) mtbdd_getvalue(b);

        cnum res_data;
        mpz_init(res_data.a);
        mpz_init(res_data.b);
        mpz_init(res_data.c);
        mpz_init(res_data.d);
        mpz_init(res_data.k);
        mpz_add(res_data.a, a_data->a, b_data->a);
        mpz_add(res_data.b, a_data->b, b_data->b);
        mpz_add(res_data.c, a_data->c, b_data->c);
        mpz_add(res_data.d, a_data->d, b_data->d);
        mpz_set(res_data.k, a_data->k);
        
        if (!mpz_cmp_si(res_data.a, 0) && !mpz_cmp_si(res_data.b, 0) && !mpz_cmp_si(res_data.c, 0) && !mpz_cmp_si(res_data.d, 0)) {
            mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
            return (MTBDD)NULL;
        }
        
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
        return res;
    }

    /* Commutative, so swap a,b for better cache performance*/
    if (a < b) {
        *p_a = b;
        *p_b = a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, my_op_minus, MTBDD*, p_a, MTBDD*, p_b)
{
    MTBDD a = *p_a;
    MTBDD b = *p_b;

    // Partial function check
    if (a == mtbdd_false){
        MTBDD b_minus = mtbdd_uapply(b, TASK(my_op_negate), 0);
        return b_minus; // return -b
    } 
    if (b == mtbdd_false) return a;

    // Compute a - b if both mtbdds are leaves
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);
        cnum* b_data = (cnum*) mtbdd_getvalue(b);

        cnum res_data;
        mpz_init(res_data.a);
        mpz_init(res_data.b);
        mpz_init(res_data.c);
        mpz_init(res_data.d);
        mpz_init(res_data.k);
        mpz_sub(res_data.a, a_data->a, b_data->a);
        mpz_sub(res_data.b, a_data->b, b_data->b);
        mpz_sub(res_data.c, a_data->c, b_data->c);
        mpz_sub(res_data.d, a_data->d, b_data->d);
        mpz_set(res_data.k, a_data->k);
        
        if (!mpz_cmp_si(res_data.a, 0) && !mpz_cmp_si(res_data.b, 0) && !mpz_cmp_si(res_data.c, 0) && !mpz_cmp_si(res_data.d, 0)) {
            mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
            return (MTBDD)NULL;
        }
        
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, my_op_times, MTBDD*, p_a, MTBDD*, p_b)
{
    MTBDD a = *p_a;
    MTBDD b = *p_b;

    // Partial function check
    if ((a == mtbdd_false) || (b == mtbdd_false)) return mtbdd_false; // 0 * X = 0

    // Compute a * b if both mtbdds are leaves
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);
        cnum* b_data = (cnum*) mtbdd_getvalue(b);

        cnum res_data;
        mpz_init(res_data.a);
        mpz_init(res_data.b);
        mpz_init(res_data.c);
        mpz_init(res_data.d);
        mpz_init(res_data.k);
        mpz_mul(res_data.a, a_data->a, b_data->a);
        mpz_mul(res_data.b, a_data->b, b_data->b);
        mpz_mul(res_data.c, a_data->c, b_data->c);
        mpz_mul(res_data.d, a_data->d, b_data->d);
        mpz_add(res_data.k, a_data->k, b_data->k);

        if (!mpz_cmp_si(res_data.a, 0) && !mpz_cmp_si(res_data.b, 0) && !mpz_cmp_si(res_data.c, 0) && !mpz_cmp_si(res_data.d, 0)) {
            mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
            return (MTBDD)NULL;
        }

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
        return res;
    }

    /* Commutative, so swap a,b for better cache performance */
    if (a < b) {
        *p_a = b;
        *p_b = a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, my_op_negate, MTBDD, a, size_t, x)
{
    (void)x; // extra parameter needed for task - not needed

    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Compute -a if mtbdd is a leaf
    if (mtbdd_isleaf(a)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);

        cnum res_data;
        mpz_init(res_data.a);
        mpz_init(res_data.b);
        mpz_init(res_data.c);
        mpz_init(res_data.d);
        mpz_init(res_data.k);
        mpz_neg(res_data.a, a_data->a);
        mpz_neg(res_data.b, a_data->b);
        mpz_neg(res_data.c, a_data->c);
        mpz_neg(res_data.d, a_data->d);
        mpz_set(res_data.k, a_data->k);
        
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, my_op_coef_k_incr, MTBDD, a, size_t, x)
{
    (void)x; // extra parameter needed for task - not needed

    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Compute -a if mtbdd is a leaf
    if (mtbdd_isleaf(a)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);

        cnum res_data;
        mpz_init_set(res_data.a, a_data->a);
        mpz_init_set(res_data.b, a_data->b);
        mpz_init_set(res_data.c, a_data->c);
        mpz_init_set(res_data.d, a_data->d);
        mpz_init(res_data.k);
        mpz_add_ui(res_data.k, a_data->k, 1);

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, my_op_coef_rot1, MTBDD, a, size_t, x)
{
    (void)x; // extra parameter needed for task - not needed

    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Compute coeficient rotation if mtbdd is a leaf
    if (mtbdd_isleaf(a)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);

        cnum res_data;
        mpz_init(res_data.a);
        mpz_neg(res_data.a, a_data->d);
        mpz_init_set(res_data.b, a_data->a);
        mpz_init_set(res_data.c, a_data->b);
        mpz_init_set(res_data.d, a_data->c);
        mpz_init_set(res_data.k, a_data->k);

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, my_op_coef_rot2, MTBDD, a, size_t, x)
{
    (void)x; // extra parameter needed for task - not needed

    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Compute coeficient rotation if mtbdd is a leaf
    if (mtbdd_isleaf(a)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);

        cnum res_data;
        mpz_init(res_data.a);
        mpz_neg(res_data.a, a_data->c);
        mpz_init(res_data.b);
        mpz_neg(res_data.b, a_data->d);
        mpz_init_set(res_data.c, a_data->a);
        mpz_init_set(res_data.d, a_data->b);
        mpz_init_set(res_data.k, a_data->k);

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, res_data.k, NULL);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

// Operations needed for gate representation:
TASK_IMPL_2(MTBDD, t_xt_create, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Skip and return high edge if node variable is target qubit
    if (mtbdd_isnode(a)) {
        if (mtbdd_getvar(a) == (uint32_t)xt) { // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
            return mtbdd_gethigh(a);
        }
    }
    // Else copy if mtbdd is leaf
    else if (mtbdd_isleaf(a)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) a_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, t_xt_comp_create, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Skip and return low edge if node variable is target qubit
    if (mtbdd_isnode(a)) {
        if (mtbdd_getvar(a) == (uint32_t)xt) { // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
            return mtbdd_getlow(a);
        }
    }
    // Else copy if mtbdd is leaf
    else if (mtbdd_isleaf(a)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) a_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

MTBDD b_xt_create(uint32_t xt)
{
    BDDSET variables = mtbdd_set_empty();
    variables = mtbdd_set_add(variables, xt); // list of all nonterminal node names

    cnum num;
    mpz_init_set_ui(num.a, 1);
    mpz_init_set_ui(num.b, 1);
    mpz_init_set_ui(num.c, 1);
    mpz_init_set_ui(num.d, 1);
    mpz_init(num.k);

    uint8_t num_symbol[] = {1}; // symbol seq. 0/1/2 means where will leaf be (low/high/both)
    MTBDD leaf1  = mtbdd_makeleaf(ltype_id, (uint64_t) &num);
    MTBDD b_xt = mtbdd_cube(variables, num_symbol, leaf1); // creates mtbdd with leaves based on variables and symbol. seq.
    mpz_clears(num.a, num.b, num.c, num.d, num.k, NULL);
    return b_xt;
}

MTBDD b_xt_comp_create(uint32_t xt)
{
    BDDSET variables = mtbdd_set_empty();
    variables = mtbdd_set_add(variables, xt); // list of all nonterminal node names

    cnum num;
    mpz_init_set_ui(num.a, 1);
    mpz_init_set_ui(num.b, 1);
    mpz_init_set_ui(num.c, 1);
    mpz_init_set_ui(num.d, 1);
    mpz_init(num.k);

    uint8_t num_symbol_comp[] = {0}; // symbol seq. 0/1/2 means where will leaf be (low/high/both)
    MTBDD leaf  = mtbdd_makeleaf(ltype_id, (uint64_t) &num);
    MTBDD b_xt_comp = mtbdd_cube(variables, num_symbol_comp, leaf); // creates mtbdd with leaves based on variables and symbol. seq.
    mpz_clears(num.a, num.b, num.c, num.d, num.k, NULL);
    return b_xt_comp;
}

/* end of "custom_mtbdd.c" */
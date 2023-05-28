#include "custom_mtbdd.h"

/*
 * Custom leaf implementation is taken from: https://github.com/MichalHe/sylvan-custom-leaf-example
 */

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
#define MY_HASH_COMB(val, data) ( (val) ^ (my_hash(mpz_get_ui(data)) + 0x9e3779b9 + ((val)<<6) + ((val)>>2)) )

/**
 * Hash function for 64bit integers.
 * 
 * Taken from: https://stackoverflow.com/a/12996028, author: Thomas Mueller (https://stackoverflow.com/users/382763/thomas-mueller)
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
    lace_start(1, 0); // 1 thread, default task queue size
    sylvan_set_limits(2000LL*1024*1024, 3, 5); // Allocate 2GB
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

    *ldata_p = new_ldata;
}

void my_leaf_destroy(uint64_t ldata)
{
    cnum* data_p = (cnum*) ldata; // Data in leaf = pointer to my data
    mpz_clears(data_p->a, data_p->b, data_p->c, data_p->d, NULL);
    free(data_p);
}

int my_leaf_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    cnum* ldata_a = (cnum*) ldata_a_raw;
    cnum* ldata_b = (cnum*) ldata_b_raw;

    return !mpz_cmp(ldata_a->a, ldata_b->a) && !mpz_cmp(ldata_a->b, ldata_b->b) && !mpz_cmp(ldata_a->c, ldata_b->c) \
           && !mpz_cmp(ldata_a->d, ldata_b->d);
}

char* my_leaf_to_str(int complemented, uint64_t ldata_raw, char* sylvan_buf, size_t sylvan_bufsize)
{
    (void) complemented;
    cnum* ldata = (cnum*) ldata_raw;

    char ldata_string[MAX_LEAF_STR_LEN] = {0};
    
    int chars_written = gmp_snprintf(ldata_string, MAX_LEAF_STR_LEN, "(1/√2)^(%Zd) * (%Zd%+Zdω%+Zdω²%+Zdω³)", c_k, ldata->a, ldata->b, ldata->c, ldata->d);
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
    val = MY_HASH_COMB(val, c_k);

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
        mpz_add(res_data.a, a_data->a, b_data->a);
        mpz_add(res_data.b, a_data->b, b_data->b);
        mpz_add(res_data.c, a_data->c, b_data->c);
        mpz_add(res_data.d, a_data->d, b_data->d);
        
        if (!mpz_cmp_si(res_data.a, 0) && !mpz_cmp_si(res_data.b, 0) && !mpz_cmp_si(res_data.c, 0) && !mpz_cmp_si(res_data.d, 0)) {
            mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
            return (MTBDD)NULL;
        }
        
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
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
        mpz_sub(res_data.a, a_data->a, b_data->a);
        mpz_sub(res_data.b, a_data->b, b_data->b);
        mpz_sub(res_data.c, a_data->c, b_data->c);
        mpz_sub(res_data.d, a_data->d, b_data->d);
        
        if (!mpz_cmp_si(res_data.a, 0) && !mpz_cmp_si(res_data.b, 0) && !mpz_cmp_si(res_data.c, 0) && !mpz_cmp_si(res_data.d, 0)) {
            mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
            return (MTBDD)NULL;
        }
        
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
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
        mpz_mul(res_data.a, a_data->a, b_data->a);
        mpz_mul(res_data.b, a_data->b, b_data->b);
        mpz_mul(res_data.c, a_data->c, b_data->c);
        mpz_mul(res_data.d, a_data->d, b_data->d);

        if (!mpz_cmp_si(res_data.a, 0) && !mpz_cmp_si(res_data.b, 0) && !mpz_cmp_si(res_data.c, 0) && !mpz_cmp_si(res_data.d, 0)) {
            mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
            return (MTBDD)NULL;
        }

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
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
        mpz_neg(res_data.a, a_data->a);
        mpz_neg(res_data.b, a_data->b);
        mpz_neg(res_data.c, a_data->c);
        mpz_neg(res_data.d, a_data->d);
        
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
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

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
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

        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        mpz_clears(res_data.a, res_data.b, res_data.c, res_data.d, NULL);
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
        cnum* a_data = (cnum*) mtbdd_getvalue(a); //FIXME: why not just return a

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
    else if (mtbdd_isleaf(a)) { // TODO: just else ??
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

    uint8_t num_symbol[] = {1}; // symbol seq. 0/1/2 denotes where the leaf will be (low/high/both)
    MTBDD leaf1  = mtbdd_makeleaf(ltype_id, (uint64_t) &num);
    MTBDD b_xt = mtbdd_cube(variables, num_symbol, leaf1); // creates mtbdd with leaves based on variables and symbol. seq.
    mpz_clears(num.a, num.b, num.c, num.d, NULL);
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

    uint8_t num_symbol_comp[] = {0}; // symbol seq. 0/1/2 denotes where the leaf will be (low/high/both)
    MTBDD leaf  = mtbdd_makeleaf(ltype_id, (uint64_t) &num);
    MTBDD b_xt_comp = mtbdd_cube(variables, num_symbol_comp, leaf); // creates mtbdd with leaves based on variables and symbol. seq.
    mpz_clears(num.a, num.b, num.c, num.d, NULL);
    return b_xt_comp;
}

// TODO: comment + placement?
#define get_coef(start, end, target) \
        (end > target && start < target)? (1 << (end - start - 2)) : (1 << (end - start -1));

TASK_IMPL_4(cnum*, mtbdd_apply_sum, MTBDD, a, uint32_t, xt, char*, curr_state, int, n)
{
    cnum* res = malloc(sizeof(cnum));
    mpz_inits(res->a, res->b, res->c, res->d, NULL);

    //TODO: cache + gc ??

    if (a == mtbdd_false) {
        mpz_set_ui(res->a, 0);
        mpz_set_ui(res->b, 0);
        mpz_set_ui(res->c, 0);
        mpz_set_ui(res->d, 0);
        return res; //FIXME: sjednotit returny??
    }

    // terminal case
    if (mtbdd_isleaf(a)) {
        cnum* a_data = (cnum*) mtbdd_getvalue(a);
        mpz_set(res->a, a_data->a);
        mpz_set(res->b, a_data->b);
        mpz_set(res->c, a_data->c);
        mpz_set(res->d, a_data->d);
        return res;
    }

    uint32_t var_a = mtbdd_getvar(a);

    //FIXME: should calculate everything here????
    MTBDD high = mtbdd_gethigh(a);
    uint32_t var_high;
    if (mtbdd_isleaf(high)) {
        var_high = n;
    }
    else {
        var_high = mtbdd_getvar(high); //TODO: check behaviour for mtbdd_false
    }
    long skip_high = get_coef(var_a, var_high, xt);

    MTBDD low = mtbdd_getlow(a);
    uint32_t var_low;
    if (mtbdd_isleaf(low)) {
        var_low = n;
    }
    else {
        var_low = mtbdd_getvar(low); //TODO: check behaviour for mtbdd_false
    }
    long skip_low = get_coef(var_a, var_low, xt);

    if (var_a == xt || curr_state[var_a] == '1') {
        cnum* res_high = CALL(mtbdd_apply_sum, high, xt, curr_state, n);
        mpz_mul_si(res->a, res_high->a, skip_high); //TODO: rework as shifts - skip_high will be the number of shifts <<
        mpz_mul_si(res->b, res_high->b, skip_high); //TODO: change to ui ??
        mpz_mul_si(res->c, res_high->c, skip_high);
        mpz_mul_si(res->d, res_high->d, skip_high);

        // cleanup of the returned value
        mpz_clears(res_high->a, res_high->b, res_high->c, res_high->d, NULL);
        free(res_high);
    }
    else if (curr_state[var_a] == '0') {
        cnum* res_low = CALL(mtbdd_apply_sum, low, xt, curr_state, n);
        mpz_mul_si(res->a, res_low->a, skip_low);
        mpz_mul_si(res->b, res_low->b, skip_low);
        mpz_mul_si(res->c, res_low->c, skip_low);
        mpz_mul_si(res->d, res_low->d, skip_low);

        // cleanup of the returned value
        mpz_clears(res_low->a, res_low->b, res_low->c, res_low->d, NULL);
        free(res_low);
    }
    else {
        SPAWN(mtbdd_apply_sum, high, xt, curr_state, n);
        cnum* res_low = CALL(mtbdd_apply_sum, low, xt, curr_state, n);
        mpz_mul_si(res_low->a, res_low->a, skip_low);
        mpz_mul_si(res_low->b, res_low->b, skip_low);
        mpz_mul_si(res_low->c, res_low->c, skip_low);
        mpz_mul_si(res_low->d, res_low->d, skip_low);

        cnum* res_high = SYNC(mtbdd_apply_sum);

        mpz_mul_si(res_high->a, res_high->a, skip_high);
        mpz_mul_si(res_high->b, res_high->b, skip_high);
        mpz_mul_si(res_high->c, res_high->c, skip_high);
        mpz_mul_si(res_high->d, res_high->d, skip_high);

        mpz_add(res->a, res_high->a, res_low->a);
        mpz_add(res->b, res_high->b, res_low->b);
        mpz_add(res->c, res_high->c, res_low->c);
        mpz_add(res->d, res_high->d, res_low->d);

        // cleanup of returned values
        mpz_clears(res_high->a, res_high->b, res_high->c, res_high->d, NULL);
        free(res_high);
        mpz_clears(res_low->a, res_low->b, res_low->c, res_low->d, NULL);
        free(res_low);
    }

    return res;
}

void my_mtbdd_leaf_sum_wrapper(MTBDD a, cnum *prob_sum, uint32_t xt, char *curr_state, int n)
{
    uint32_t var_a;
    if (mtbdd_isleaf(a)) {
        var_a = n;
    }
    else {
        var_a = mtbdd_getvar(a); //TODO: check behaviour for mtbdd_false
    }
    long skip_coef = 1; //FIXME:2 * get_coef(0, var_a, xt);
    cnum* sum = RUN(mtbdd_apply_sum, a, xt, curr_state, n);
    mpz_mul_si(prob_sum->a, sum->a, skip_coef);
    mpz_mul_si(prob_sum->b, sum->b, skip_coef);
    mpz_mul_si(prob_sum->c, sum->c, skip_coef);
    mpz_mul_si(prob_sum->d, sum->d, skip_coef);

    // cleanup of the returned value
    mpz_clears(sum->a, sum->b, sum->c, sum->d, NULL);
    free(sum);
}

/* end of "custom_mtbdd.c" */
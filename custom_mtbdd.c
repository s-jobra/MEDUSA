#include "custom_mtbdd.h"

/**
 * Global variable for my custom leaf type id
 */
uint32_t ltype_id;

/* INTERNAL */
/**
 * Hash value and combine it with an already existing hash
 * Values from: http://www.boost.org/doc/libs/1_64_0/boost/functional/hash/hash.hpp
 */
#define MY_HASH_COMB(val, data) ( (val) ^ (my_hash((uint64_t)(data)) + 0x9e3779b9 + ((val)<<6) + ((val)>>2)) )
/**
 * Hash function for 64bit integers.
 * Taken from: https://stackoverflow.com/a/12996028
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

/* ERROR HANDLING */
void error_exit(const char *error)
{
    // create error message format
    unsigned error_len = strlen(error) + strlen(ERROR_TEXT);
    char msg[error_len + 1];
    strcpy(msg, ERROR_TEXT);
    strcat(msg, error);

    fprintf(stderr, "%s\n",msg);
    exit(1);
}

/* SETUP */
void init_sylvan() {
    lace_start(1, 0); // 1 thread, default task queue size
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

    new_ldata->a = orig_ldata->a;
    new_ldata->b = orig_ldata->b;
    new_ldata->c = orig_ldata->c;
    new_ldata->d = orig_ldata->d;
    new_ldata->k = orig_ldata->k;

    *ldata_p = new_ldata;
}

void my_leaf_destroy(uint64_t ldata)
{
    cnum* data_p = (cnum*) ldata; // Data in leaf = pointer to my data
    free(data_p);
}

int my_leaf_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    cnum* ldata_a = (cnum*) ldata_a_raw;
    cnum* ldata_b = (cnum*) ldata_b_raw;

    // !todo makro
    return (ldata_a->a == ldata_b->a) && (ldata_a->b == ldata_b->b) && (ldata_a->c == ldata_b->c) && (ldata_a->d == ldata_b->d) && (ldata_a->k == ldata_b->k);
}

char* my_leaf_to_str(int complemented, uint64_t ldata_raw, char* sylvan_buf, size_t sylvan_bufsize)
{
    // ?? proc complemented k nicemu ??
    (void) complemented;
    cnum* ldata = (cnum*) ldata_raw;

    // !todo makro na velikost
    char ldata_string[100] = {0};
    // ?? resit zapornou navrat. hodnotu a vel. > 100 ??
    int chars_written = snprintf(ldata_string, 100, "(1/√2)^(%ld) * (%ld%+ldω%+ldω²%+ldω³)", ldata->k, ldata->a, ldata->b, ldata->c, ldata->d);

    // ?? je potreba null termination ??
    // Is buffer large enough?
    if (chars_written < sylvan_bufsize) {
        memcpy(sylvan_buf, ldata_string, chars_written * sizeof(char));
        sylvan_buf[chars_written] = '\0';
        return sylvan_buf;
    }
    
    // Else return newly allocated string
    char* new_buf = (char*)my_malloc(chars_written * sizeof(char));
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

        //FIXME: overflow/underflow solve:
        if ((a_data->a > 0 && b_data->a > INT64_MAX - a_data->a) || (a_data->a < 0 && b_data->a < INT64_MIN - a_data->a) ||
            (a_data->b > 0 && b_data->b > INT64_MAX - a_data->b) || (a_data->b < 0 && b_data->b < INT64_MIN - a_data->b) ||
            (a_data->c > 0 && b_data->c > INT64_MAX - a_data->c) || (a_data->c < 0 && b_data->c < INT64_MIN - a_data->c) ||
            (a_data->d > 0 && b_data->d > INT64_MAX - a_data->d) || (a_data->d < 0 && b_data->d < INT64_MIN - a_data->d) ) {
            error_exit("Integer overflow or underflow occured in one of the coefficients.");
        }
        else {
            cnum res_data = {.a = a_data->a + b_data->a, \
                            .b = a_data->b + b_data->b, \
                            .c = a_data->c + b_data->c, \
                            .d = a_data->d + b_data->d, \
                            .k = a_data->k};               // ?? ma se k menit pri scitani nebo ne ??
            MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
            return res;
        }
    }

    // ?? vykon ??
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

        //FIXME: overflow/underflow solve:
        if ((b_data->a < 0 && a_data->a > INT64_MAX + b_data->a) || (b_data->a > 0 && a_data->a < INT64_MIN + b_data->a) ||
            (b_data->b < 0 && a_data->b > INT64_MAX + b_data->b) || (b_data->b > 0 && a_data->b < INT64_MIN + b_data->b) ||
            (b_data->c < 0 && a_data->c > INT64_MAX + b_data->c) || (b_data->c > 0 && a_data->c < INT64_MIN + b_data->c) ||
            (b_data->d < 0 && a_data->d > INT64_MAX + b_data->d) || (b_data->d > 0 && a_data->d < INT64_MIN + b_data->d) ) {
            error_exit("Integer overflow or underflow occured in one of the coefficients.");
        }
        else {
            cnum res_data = {.a = a_data->a - b_data->a, \
                            .b = a_data->b - b_data->b, \
                            .c = a_data->c - b_data->c, \
                            .d = a_data->d - b_data->d, \
                            .k = a_data->k};
            MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
            return res;
        }
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

        //FIXME: overflow/underflow solve:
        if ((a_data->a > 0 && b_data->a > 0 && a_data->a > INT64_MAX / b_data->a) || (a_data->b > 0 && b_data->b > 0 && a_data->b > INT64_MAX / b_data->b) ||
            (a_data->c > 0 && b_data->c > 0 && a_data->c > INT64_MAX / b_data->c) || (a_data->c > 0 && b_data->d > 0 && a_data->c > INT64_MAX / b_data->d) ||
            
            (a_data->a > 0 && b_data->a < 0 && b_data->a < INT64_MIN / a_data->a) || (a_data->b > 0 && b_data->b < 0 && b_data->b > INT64_MIN / a_data->b) ||
            (a_data->c > 0 && b_data->c < 0 && b_data->c < INT64_MIN / a_data->c) || (a_data->c > 0 && b_data->d < 0 && b_data->c > INT64_MIN / a_data->d) ||
            
            (a_data->a < 0 && b_data->a > 0 && a_data->a < INT64_MIN / b_data->a) || (a_data->b < 0 && b_data->b > 0 && a_data->b < INT64_MIN / b_data->b) ||
            (a_data->c < 0 && b_data->c > 0 && a_data->c < INT64_MIN / b_data->c) || (a_data->c < 0 && b_data->d > 0 && a_data->c < INT64_MIN / b_data->d) ||
            
            (a_data->a < 0 && b_data->a < 0 && b_data->a < INT64_MAX / a_data->a) || (a_data->b < 0 && b_data->b < 0 && b_data->b < INT64_MAX / a_data->b) ||
            (a_data->c < 0 && b_data->c < 0 && b_data->c < INT64_MAX / a_data->c) || (a_data->c < 0 && b_data->d < 0 && b_data->c < INT64_MAX / a_data->d)) {
            error_exit("Integer overflow or underflow occured in one of the coefficients.");
        }
        else {
            cnum res_data = {.a = a_data->a * b_data->a, \
                             .b = a_data->b * b_data->b, \
                             .c = a_data->c * b_data->c, \
                             .d = a_data->d * b_data->d, \
                             .k = a_data->k + b_data->k};
            MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
            return res;
        }
    }

    // ?? vykon ??
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

        cnum res_data = {.a = -a_data->a, \
                         .b = -a_data->b, \
                         .c = -a_data->c, \
                         .d = -a_data->d, \
                         .k = a_data->k};
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
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

        cnum res_data = {.a = a_data->a, \
                         .b = a_data->b, \
                         .c = a_data->c, \
                         .d = a_data->d, \
                         .k = a_data->k + 1};
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
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

        cnum res_data = {.a = -(a_data->d), \
                         .b = a_data->a, \
                         .c = a_data->b, \
                         .d = a_data->c, \
                         .k = a_data->k};
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
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

        cnum res_data = {.a = -(a_data->c), \
                         .b = -(a_data->d), \
                         .c = a_data->a, \
                         .d = a_data->b, \
                         .k = a_data->k};
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
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

    cnum num1 = {.a = 1, .b = 1, .c = 1, .d = 1, .k = 0};
    uint8_t num1_symbol[] = {1}; // symbol seq. 0/1/2 means where will leaf be (low/high/both)
    MTBDD leaf1  = mtbdd_makeleaf(ltype_id, (uint64_t) &num1);
    MTBDD b_xt = mtbdd_cube(variables, num1_symbol, leaf1); // creates mtbdd with leaves based on variables and symbol. seq.
    return b_xt;
}

MTBDD b_xt_comp_create(uint32_t xt)
{
    BDDSET variables = mtbdd_set_empty();
    variables = mtbdd_set_add(variables, xt); // list of all nonterminal node names

    cnum num1 = {.a = 1, .b = 1, .c = 1, .d = 1, .k = 0};
    uint8_t num1_symbol_comp[] = {0}; // symbol seq. 0/1/2 means where will leaf be (low/high/both)
    MTBDD leaf1  = mtbdd_makeleaf(ltype_id, (uint64_t) &num1);
    MTBDD b_xt_comp = mtbdd_cube(variables, num1_symbol_comp, leaf1); // creates mtbdd with leaves based on variables and symbol. seq.
    return b_xt_comp;
}

/* end of "custom_mtbdd.c" */
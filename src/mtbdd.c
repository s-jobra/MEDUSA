#include "mtbdd.h"

/*
 * Custom leaf implementation is taken from: https://github.com/MichalHe/sylvan-custom-leaf-example
 */

/**
 * Max. size of string written as leaf value in output file.
 */
#define MAX_LEAF_STR_LEN 32000

uint32_t ltype_id;

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
void my_leaf_create(uint64_t *ldata_p_raw)
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
    cnum *data_p = (cnum*) ldata; // Data in leaf = pointer to my data
    mpz_clears(data_p->a, data_p->b, data_p->c, data_p->d, NULL);
    free(data_p);
}

int my_leaf_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    cnum *ldata_a = (cnum*) ldata_a_raw;
    cnum *ldata_b = (cnum*) ldata_b_raw;

    return !mpz_cmp(ldata_a->a, ldata_b->a) && !mpz_cmp(ldata_a->b, ldata_b->b) && !mpz_cmp(ldata_a->c, ldata_b->c) \
           && !mpz_cmp(ldata_a->d, ldata_b->d);
}

char* my_leaf_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize)
{
    (void) complemented;
    cnum *ldata = (cnum*) ldata_raw;

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
    char *new_buf = (char*)my_malloc((chars_written + 1) * sizeof(char));
    memcpy(new_buf, ldata_string, chars_written * sizeof(char));
    new_buf[chars_written] = '\0';
    return new_buf;
}

uint64_t my_leaf_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    cnum *ldata = (cnum*) ldata_raw;

    uint64_t val = seed;
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
        cnum *a_data = (cnum*) mtbdd_getvalue(a);
        cnum *b_data = (cnum*) mtbdd_getvalue(b);

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
        cnum *a_data = (cnum*) mtbdd_getvalue(a);
        cnum *b_data = (cnum*) mtbdd_getvalue(b);

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
        cnum *a_data = (cnum*) mtbdd_getvalue(a);
        cnum *b_data = (cnum*) mtbdd_getvalue(b);

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
        cnum *a_data = (cnum*) mtbdd_getvalue(a);

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
        cnum *a_data = (cnum*) mtbdd_getvalue(a);

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
        cnum *a_data = (cnum*) mtbdd_getvalue(a);

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
    else {
        return a;
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
    else {
        return a;
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

//TODO: change to makro?
static inline prob_t calculate_prob(cnum* prob)
{

    mpf_t a, b, c, d;
    mpf_inits(a, b, c, d, NULL);
    mpf_set_z(a, prob->a);
    mpf_set_z(b, prob->b);
    mpf_set_z(c, prob->c);
    mpf_set_z(d, prob->d);

    prob_t prob_re, prob_im;
    prob_t c_a, c_b, c_c, c_d;

    mp_bitcnt_t shift_cnt = mpz_get_ui(c_k); // TODO: check possible bottleneck

    // k even, k+1 odd
    if (mpz_even_p(c_k) != 0) {
        shift_cnt = shift_cnt >> 1;

        mpf_div_2exp(a, a, shift_cnt); // k/2 right shifts
        mpf_div_2exp(b, b, shift_cnt);
        mpf_div_2exp(c, c, shift_cnt);
        mpf_div_2exp(d, d, shift_cnt);

        c_a = mpf_get_d(a);
        c_b = mpf_get_d(b);
        c_c = mpf_get_d(c);
        c_d = mpf_get_d(d);

        prob_re = pow(c_a + c_b * M_SQRT1_2 - c_d * M_SQRT1_2, 2);
        prob_im = pow(c_c + c_b * M_SQRT1_2 + c_d * M_SQRT1_2, 2);

    }
    // k odd, k+1 even
    else {
        shift_cnt = (shift_cnt - 1) >> 1;

        mpf_div_2exp(a, a, shift_cnt); // k-1/2 right shifts
        mpf_div_2exp(b, b, shift_cnt + 1); // k+1/2 right shifts
        mpf_div_2exp(c, c, shift_cnt);
        mpf_div_2exp(d, d, shift_cnt + 1);

        c_a = mpf_get_d(a);
        c_b = mpf_get_d(b);
        c_c = mpf_get_d(c);
        c_d = mpf_get_d(d);

        prob_re = pow(c_a * M_SQRT1_2 + c_b - c_d, 2);
        prob_im = pow(c_c * M_SQRT1_2 + c_b + c_d, 2);
    }

    mpf_clears(a, b, c, d, NULL);
    return prob_re+prob_im;
}

TASK_IMPL_4(prob_t, mtbdd_prob_sum, MTBDD, a, uint32_t, xt, char*, curr_state, int, n)
{
    // we must immediately convert to float else the skip coefficient will be also squared
    prob_t res = 0;

    //TODO: cache + gc ??

    // terminal cases
    if (a == mtbdd_false) {
        return res;
    }

    if (mtbdd_isleaf(a)) {
        cnum *a_data = (cnum*) mtbdd_getvalue(a);
        res = calculate_prob(a_data);
        return res;
    }

    // else node
    uint32_t var_a = mtbdd_getvar(a);

    MTBDD high = mtbdd_gethigh(a);
    uint32_t var_high;
    if (mtbdd_isleaf(high)) {
        var_high = n;
    }
    else {
        var_high = mtbdd_getvar(high);
    }
    long skip_high = get_coef(var_a, var_high, xt);

    MTBDD low = mtbdd_getlow(a);
    uint32_t var_low;
    if (mtbdd_isleaf(low)) {
        var_low = n;
    }
    else {
        var_low = mtbdd_getvar(low);
    }
    long skip_low = get_coef(var_a, var_low, xt);

    // recursion
    if (var_a == xt || curr_state[var_a] == '1') {
        res = CALL(mtbdd_prob_sum, high, xt, curr_state, n);
        res *= skip_high;
    }
    else if (curr_state[var_a] == '0') {
        res = CALL(mtbdd_prob_sum, low, xt, curr_state, n);
        res *= skip_low;
    }
    else {
        SPAWN(mtbdd_prob_sum, high, xt, curr_state, n);
        prob_t res_low = CALL(mtbdd_prob_sum, low, xt, curr_state, n);
        res_low *= skip_low;

        prob_t res_high = SYNC(mtbdd_prob_sum);
        res_high *= skip_high;

        res = res_low + res_high;
    }

    return res;
}

/* end of "mtbdd.c" */
#include <sylvan.h>
#include <string.h>

uint32_t ltype_id;

/**
 * Complex number in algebraic representation
 */
typedef struct cnum {
    int a;
    int b;
    int c;
    int d;
    int k;
}cnum;

/**
 * Custom malloc function including error handling.
 */
void* my_malloc(size_t size)
{
    void* p = malloc(size);
    if (p == NULL) {
        abort();
    }

    return p;
}

/**
 * Handle called when my new custom leaf is created and is not found in the internal table.
 * It allocates and initializes custom leaf data from the given data pointer.
 */
void my_leaf_create(uint64_t* ldata_p_raw)
{
    cnum** ldata_p = (cnum**)ldata_p_raw; // Leaf data type is uint64_t, we store there ptr to our actual data
    
    cnum* orig_ldata = *ldata_p;
    cnum* new_ldata = (cnum*)my_malloc(sizeof(cnum));
    // !todo makro
    new_ldata->a = orig_ldata->a;
    new_ldata->b = orig_ldata->b;
    new_ldata->c = orig_ldata->c;
    new_ldata->d = orig_ldata->d;
    new_ldata->k = orig_ldata->k;

    *ldata_p = new_ldata;
}

/**
 * Handle called when my custom leaf is destroyed during garbage collection.
 */
void my_leaf_destroy(uint64_t ldata)
{
    cnum* data_p = (cnum*) ldata; // Data in leaf = pointer to my data
    free(data_p);
}

/**
 * Handle called when comparing two custom leaves.
 */
int my_leaf_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    cnum* ldata_a = (cnum*) ldata_a_raw;
    cnum* ldata_b = (cnum*) ldata_b_raw;

    // !todo makro
    return (ldata_a->a == ldata_b->a) && (ldata_a->b == ldata_b->b) && (ldata_a->c == ldata_b->c) && (ldata_a->d == ldata_b->d) && (ldata_a->k == ldata_b->k);
}

// ??? todo change na c++ hash, test include ??
static inline uint64_t my_hash(uint64_t x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

// ??? rychlost vs citelnost ??
// ??? predelat nejak dal ??
#define MY_HASH_COMB(val, data) ( (val) ^ (my_hash((uint64_t)(data)) + 0x9e3779b9 + ((val)<<6) + ((val)>>2)) )

/**
 * Hashing function for calculating leaf's hash.
 */
uint64_t my_leaf_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    (void)seed;
    cnum* ldata = (cnum*) ldata_raw;

    uint64_t val = 0;
    val = MY_HASH_COMB(val, ldata->a);
    val = MY_HASH_COMB(val, ldata->b);
    val = MY_HASH_COMB(val, ldata->c);
    val = MY_HASH_COMB(val, ldata->d);
    val = MY_HASH_COMB(val, ldata->k);

    return seed * val;
}

/**
 * Handle for creating string representation of leaf.
 */
char* my_leaf_to_str(int complemented, uint64_t ldata_raw, char* sylvan_buf, size_t sylvan_bufsize)
{
    // ?? proc complemented k nicemu ??
    (void) complemented;
    cnum* ldata = (cnum*) ldata_raw;

    // !todo makro na velikost
    char ldata_string[100] = {0};
    // ?? resit zapornou navrat. hodnotu a vel. > 100 ??
    int chars_written = snprintf(ldata_string, 100, "(1/√2)^(%d) * (%d%+dω%+dω²%+dω³)", ldata->k, ldata->a, ldata->b, ldata->c, ldata->d);

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

/**
 * Function for sylvan and lace setup.
 */
void init_sylvan()
{
    lace_start(1, 0); // 1 thread, default task queue size
    sylvan_set_limits(500LL*1024*1024, 3, 5); // Allocate 100MB
    sylvan_init_package();
    sylvan_init_mtbdd();
}

/**
 * Function for my custom leaf setup.
 */
uint32_t init_my_leaf()
{
    uint32_t my_leaf_type_id = sylvan_mt_create_type();

    sylvan_mt_set_hash(my_leaf_type_id, my_leaf_hash);
    sylvan_mt_set_equals(my_leaf_type_id, my_leaf_equals);
    sylvan_mt_set_create(my_leaf_type_id, my_leaf_create);
    sylvan_mt_set_destroy(my_leaf_type_id, my_leaf_destroy);
    sylvan_mt_set_to_str(my_leaf_type_id, my_leaf_to_str);

    return my_leaf_type_id;
}

// Basic operation declarations
TASK_DECL_2(MTBDD, my_op_plus, MTBDD*, MTBDD*);
TASK_DECL_2(MTBDD, my_op_minus, MTBDD*, MTBDD*);
TASK_DECL_2(MTBDD, my_op_times, MTBDD*, MTBDD*);
TASK_DECL_2(MTBDD, my_op_negate, MTBDD, size_t);
TASK_DECL_2(MTBDD, my_op_k_increment, MTBDD, size_t);
TASK_DECL_2(MTBDD, t_xt_create, MTBDD, uint64_t);
TASK_DECL_2(MTBDD, t_xt_comp_create, MTBDD, uint64_t);

/**
 * Operation plus for my custom MTBDDs.
 */
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

        cnum res_data = {.a = a_data->a + b_data->a, \
                         .b = a_data->b + b_data->b, \
                         .c = a_data->c + b_data->c, \
                         .d = a_data->d + b_data->d, \
                         .k = a_data->k};               // ?? ma se k menit pri scitani nebo ne ??
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        return res;
    }

    // ?? vykon ??
    /* Commutative, so swap a,b for better cache performance
    if (a < b) {
        *pa = b;
        *pb = a;
    }*/

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Operation minus for my custom MTBDDs.
 */
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

        cnum res_data = {.a = a_data->a - b_data->a, \
                         .b = a_data->b - b_data->b, \
                         .c = a_data->c - b_data->c, \
                         .d = a_data->d - b_data->d, \
                         .k = a_data->k - b_data->k};
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Operation times for my custom MTBDDs.
 */
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

        cnum res_data = {.a = a_data->a * b_data->a, \
                         .b = a_data->b * b_data->b, \
                         .c = a_data->c * b_data->c, \
                         .d = a_data->d * b_data->d, \
                         .k = a_data->k * b_data->k};
        MTBDD res = mtbdd_makeleaf(ltype_id, (uint64_t) &res_data);
        return res;
    }

    // ?? vykon ??
    /* Commutative, so swap a,b for better cache performance
    if (a < b) {
        *pa = b;
        *pb = a;
    }*/

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Operation times with constant -1 for my custom MTBDD.
 */
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

/**
 * Operation times with constant 1/√2 for my custom MTBDD.
 */
TASK_IMPL_2(MTBDD, my_op_k_increment, MTBDD, a, size_t, x)
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


/**
 * Function for creating MTBDD for a target qubit.
 * (low -> 0, high -> 1)
 */
MTBDD b_xt_create(uint32_t xt)
{
    BDDSET variables = mtbdd_set_empty();
    variables = mtbdd_set_add(variables, xt);

    cnum num1 = {.a = 1, .b = 1, .c = 1, .d = 1, .k = 1};
    uint8_t num1_symbol[] = {1};
    MTBDD leaf1  = mtbdd_makeleaf(ltype_id, (uint64_t) &num1);
    MTBDD b_xt = mtbdd_cube(variables, num1_symbol, leaf1);
    return b_xt;
}

/**
 * Function for creating MTBDD for a target qubit complement.
 * (low -> 1, high -> 0)
 */
MTBDD b_xt_comp_create(uint32_t xt)
{
    BDDSET variables = mtbdd_set_empty();
    variables = mtbdd_set_add(variables, xt);

    cnum num1 = {.a = 1, .b = 1, .c = 1, .d = 1, .k = 1};
    uint8_t num1_symbol_comp[] = {0};
    MTBDD leaf1  = mtbdd_makeleaf(ltype_id, (uint64_t) &num1);
    MTBDD b_xt_comp = mtbdd_cube(variables, num1_symbol_comp, leaf1);
    return b_xt_comp;
}

// ?? rework na pointer ??
/**
 * Function for creating a new MTBDD from a given one with restriction: target qubit = 1.
 */
TASK_IMPL_2(MTBDD, t_xt_create, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Skip and return high edge if node variable is target qubit
    if (mtbdd_isnode(a)) {
        if (mtbdd_getvar(a) == (uint32_t)xt) {
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

/**
 * Function for creating a new MTBDD from a given one with restriction: target qubit = 0.
 */
TASK_IMPL_2(MTBDD, t_xt_comp_create, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Skip and return low edge if node variable is target qubit
    if (mtbdd_isnode(a)) {
        if (mtbdd_getvar(a) == (uint32_t)xt) {
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

/**
 * Function implementing quantum gate X for given MTBDD.
 */
MTBDD gate_x(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    MTBDD res;

    MTBDD b_xt = b_xt_create(xt);
    MTBDD t_xt_comp = mtbdd_uapply(t, TASK(t_xt_comp_create), xt);
    res = mtbdd_apply(b_xt, t_xt_comp, TASK(my_op_times));

    MTBDD b_xt_comp = b_xt_comp_create(xt);
    MTBDD t_xt = mtbdd_uapply(t, TASK(t_xt_create), xt);
    MTBDD res_temp = mtbdd_apply(b_xt_comp, t_xt, TASK(my_op_times));

    res = mtbdd_apply(res, res_temp, TASK(my_op_plus));
    return res;
}

/**
 * Function implementing quantum gate Z for given MTBDD.
 */
MTBDD gate_z(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    MTBDD res;

    MTBDD b_xt_comp = b_xt_comp_create(xt);
    res = mtbdd_apply(b_xt_comp, t, TASK(my_op_times));

    MTBDD b_xt = b_xt_create(xt);
    MTBDD res_temp = mtbdd_apply(b_xt, t, TASK(my_op_times));

    res = mtbdd_apply(res, res_temp, TASK(my_op_minus));
    return res;
}

/**
 * Function implementing quantum Hadamard gate for given MTBDD.
 */
MTBDD gate_h(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    MTBDD res;

    res = mtbdd_uapply(t, TASK(t_xt_comp_create), xt);  // t_xt_comp

    MTBDD b_xt_comp = b_xt_comp_create(xt);
    MTBDD t_xt = mtbdd_uapply(t, TASK(t_xt_create), xt);
    MTBDD res_temp = mtbdd_apply(b_xt_comp, t_xt, TASK(my_op_times));

    res = mtbdd_apply(res, res_temp, TASK(my_op_plus));

    MTBDD b_xt = b_xt_create(xt);
    res_temp = mtbdd_apply(b_xt, t, TASK(my_op_times));

    res = mtbdd_apply(res, res_temp, TASK(my_op_minus));

    res = mtbdd_uapply(res, TASK(my_op_k_increment), 0);
    return res;
}

/**
 * Function implementing quantum CNOT gate for given MTBDD.
 */
MTBDD gate_cnot(MTBDD* p_t, uint32_t xt, uint32_t xc)
{
    MTBDD t = *p_t;
    MTBDD res;

    MTBDD b_xc_comp = b_xt_comp_create(xc);
    res = mtbdd_apply(b_xc_comp, t, TASK(my_op_times)); // b_xc_comp * t

    MTBDD b_xc = b_xt_create(xc);
    MTBDD b_xt_comp = b_xt_comp_create(xt);
    MTBDD t_xt = mtbdd_uapply(t, TASK(t_xt_create), xt);
    MTBDD res_temp = mtbdd_apply(b_xc, b_xt_comp, TASK(my_op_times)); // b_xc * b_xt_comp
    res_temp = mtbdd_apply(res_temp, t_xt, TASK(my_op_times)); // b_xc * b_xt_comp * t_xt

    res = mtbdd_apply(res, res_temp, TASK(my_op_plus)); // b_xc_comp * t + b_xc * b_xt_comp * t_xt

    MTBDD b_xt = b_xt_create(xt);
    MTBDD t_xt_comp = mtbdd_uapply(t, TASK(t_xt_comp_create), xt);
    res_temp = mtbdd_apply(b_xc, b_xt, TASK(my_op_times)); // b_xc * b_xt
    res_temp = mtbdd_apply(res_temp, t_xt_comp, TASK(my_op_times)); // b_xc * b_xt * t_xt_comp

    res = mtbdd_apply(res, res_temp, TASK(my_op_plus)); // b_xc_comp * t + b_xc * b_xt_comp * t_xt + b_xc * b_xt * t_xt_comp

    return res;
}

/**
 * Function implementing quantum CZ gate for given MTBDD.
 */
MTBDD gate_cz(MTBDD* p_t, uint32_t xt, uint32_t xc)
{
    MTBDD t = *p_t;
    MTBDD res;

    MTBDD b_xc_comp = b_xt_comp_create(xc);
    res = mtbdd_apply(b_xc_comp, t, TASK(my_op_times)); // b_xc_comp * t

    MTBDD b_xt_comp = b_xt_comp_create(xt);
    MTBDD res_temp = mtbdd_apply(b_xt_comp, t, TASK(my_op_times)); // b_xt_comp * t
    res = mtbdd_apply(res, res_temp, TASK(my_op_plus)); // b_xc_comp * t +  b_xt_comp * t

    res_temp = mtbdd_apply(b_xc_comp, b_xt_comp, TASK(my_op_times)); // b_xc_comp * b_xt_comp
    res_temp = mtbdd_apply(res_temp, t, TASK(my_op_times)); // b_xc_comp * b_xt_comp * t
    res = mtbdd_apply(res, res_temp, TASK(my_op_minus)); // b_xc_comp * t +  b_xt_comp * t - b_xc_comp * b_xt_comp * t

    MTBDD b_xc = b_xt_create(xc);
    MTBDD b_xt = b_xt_create(xt);
    res_temp = mtbdd_apply(b_xc, b_xt, TASK(my_op_times)); // b_xc * b_xt
    res_temp = mtbdd_apply(res_temp, t, TASK(my_op_times)); // b_xc * b_xt * t
    res = mtbdd_apply(res, res_temp, TASK(my_op_minus)); // final

    return res;
}

int main()
{
    init_sylvan();
    ltype_id = init_my_leaf(); // Custom leaf type id

   ///////////////////////////////////////////////////////// test
    BDDSET variables = mtbdd_set_empty();
    variables = mtbdd_set_add(variables, 1);
    variables = mtbdd_set_add(variables, 2);

    cnum point_a = {.a = 1, .b = 1, .c = 2, .d = 2, .k = -2}; 
    uint8_t point_a_symbol[] = {2, 1};
    MTBDD leaf_a  = mtbdd_makeleaf(ltype_id, (uint64_t) &point_a);
    MTBDD mtbdd_a = mtbdd_cube(variables, point_a_symbol, leaf_a);

    cnum point_b = {.a = 1, .b = 2, .c = 3, .d = 4, .k = 5}; 
    uint8_t point_b_symbol[] = {1, 1};
    MTBDD leaf_b  = mtbdd_makeleaf(ltype_id, (uint64_t) &point_b);
    MTBDD mtbdd_b = mtbdd_cube(variables, point_b_symbol, leaf_b);

    MTBDD btest_x2 = b_xt_create(2);
    MTBDD ttest_x2 = mtbdd_uapply(mtbdd_a, TASK(t_xt_create), 2);

    MTBDD result_add = mtbdd_apply(mtbdd_a, mtbdd_b, TASK(my_op_plus));
    MTBDD result_mul = mtbdd_apply(mtbdd_a, btest_x2, TASK(my_op_times));
    MTBDD result_sub = mtbdd_apply(mtbdd_a, mtbdd_b, TASK(my_op_minus));

    FILE* f_orig = fopen("orig.dot", "w");
    FILE* f_new = fopen("new.dot", "w");
    FILE* f_new2 = fopen("new2.dot", "w");

    /*
    printf("ADDITION:\n");
    mtbdd_fprintdot(stdout, result_add);
    printf("===============================================================\n");
    printf("MULTIPLICATION:\n");
    mtbdd_fprintdot(stdout, result_mul);
    printf("===============================================================\n");
    printf("SUBTRACTION:\n");
    mtbdd_fprintdot(stdout, result_sub);
    */
    printf("ORIGINAL:\n");
    mtbdd_fprintdot(f_orig, mtbdd_a);
    printf("===============================================================\n");
    /*
    printf("X GATE:\n");
    MTBDD result_x = gate_x(&mtbdd_a, 2);
    mtbdd_fprintdot(stdout, result_x);
    printf("===============================================================\n");
    printf("Z GATE:\n");
    MTBDD result_z = gate_z(&mtbdd_a, 1);
    mtbdd_fprintdot(stdout, result_z);
    printf("===============================================================\n");
    printf("H GATE:\n");
    MTBDD result_h = gate_h(&mtbdd_a, 1);
    mtbdd_fprintdot(stdout, result_h);
    printf("===============================================================\n"); */
    printf("CNOT GATE:\n");
    MTBDD result_cnot = gate_cnot(&mtbdd_a, 2, 1);
    mtbdd_fprintdot(f_new, result_cnot);
    printf("===============================================================\n");
    printf("CZ GATE:\n");
    MTBDD result_cz = gate_cz(&mtbdd_a, 2, 1);
    mtbdd_fprintdot(f_new2, result_cz);
    //////////////////////////////////////////////////////// test_end

    fclose(f_orig);
    fclose(f_new);
    fclose(f_new2);

    // ?? todo check x, z, h, cnot
    //    + shift gates ??

    // ?? todo hlavickove soubory + reorganizace + makefile ??
    sylvan_quit();
    lace_stop();
    return 0;
}
/* pozn:
       variables = x1, x2 apod. = seznam vsech jmen vnitrnich uzlu
       symbol sekv. 0/1/2 = pod jakyma podstromama bude uzel (low/high/oba)
       fce cube vytvori mtbdd a da list tam kam ma dle zadani

        neresit tolik implementaci
*/
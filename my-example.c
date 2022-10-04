#include <sylvan.h>
#include <string.h>

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
 * Handle called when my new custom leaf is created.
 */
void my_leaf_create(uint64_t* ldata_p_raw)
{
    // ?? error handling malloc ??

    cnum** ldata_p = (cnum**)ldata_p_raw; // Leaf data type is uint64_t, we store there ptr to our actual data
    
    // ?? proc kopie original dat ??
    cnum* orig_ldata = *ldata_p;
    cnum* new_ldata = (cnum*)malloc(sizeof(cnum));
    // !todo makro
    new_ldata->a = orig_ldata->a;
    new_ldata->b = orig_ldata->b;
    new_ldata->c = orig_ldata->c;
    new_ldata->d = orig_ldata->d;
    new_ldata->k = orig_ldata->k;
    // ?? proc prirazeni az na konci ??
    *ldata_p = new_ldata;
}

/**
 * Handle called when my custom leaf is destroyed.
 */
void my_leaf_destroy(uint64_t ldata)
{
    // ?? smazani leaf_data ??
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

/**
 * Hashing function for calculating leaf's hash.
 */
uint64_t my_leaf_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    // ?? pouzit jiny hash ??
    cnum* ldata = (cnum*) ldata_raw;
    return seed * ( ((uint64_t)ldata->a)<<32 + (uint64_t)ldata->b + (uint64_t)ldata->c + (uint64_t)ldata->d + (uint64_t)ldata->k );
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
    char* new_buf = (char*)malloc(chars_written * sizeof(char));
    memcpy(new_buf, ldata_string, chars_written * sizeof(char));
    new_buf[chars_written] = '\0';
    return new_buf;
}

////////////////////////////////////////
void init_sylvan()
{
    // ?? 1 thread nebo autodetect (0) ??
    // ?? set_limits hodnota ??
    lace_start(1, 0);
	sylvan_set_limits(500LL*1024*1024, 3, 5); // Allocate 100MB
    sylvan_init_package();
    sylvan_init_mtbdd();
}

uint32_t register_my_leaf()
{
    uint32_t my_leaf_type_id = sylvan_mt_create_type();

    // ?? proc hash (+ equals) ??

    sylvan_mt_set_hash(my_leaf_type_id, my_leaf_hash);
    sylvan_mt_set_equals(my_leaf_type_id, my_leaf_equals);
    sylvan_mt_set_create(my_leaf_type_id, my_leaf_create);
    sylvan_mt_set_destroy(my_leaf_type_id, my_leaf_destroy);
    sylvan_mt_set_to_str(my_leaf_type_id, my_leaf_to_str);

    return my_leaf_type_id;
}
////////////////////////////////////////

int main()
{
    init_sylvan();

    // ?? NOT IN GITHUB TUTORIAL ??
    // Setup lace (work-stealing used by sylvan)
    unsigned int mtbdd_worker_count = 1;
    size_t task_deque_size = 0; // Use default task queue size 
    /////////////////////////////////////////////////////////

    uint32_t my_leaf_type_id = register_my_leaf();

   ///////////////////////////////////////////////////////// test
    BDDSET variables = mtbdd_set_empty();
    variables = mtbdd_set_add(variables, 1);
    variables = mtbdd_set_add(variables, 2);

    cnum point_a = {.a = 10, .b = -2, .c = 3, .d = 4, .k = -2}; 
    uint8_t point_a_symbol[] = {1, 1};
    MTBDD leaf_a  = mtbdd_makeleaf(my_leaf_type_id, (uint64_t) &point_a);
    MTBDD mtbdd_a = mtbdd_cube(variables, point_a_symbol, leaf_a);

    mtbdd_fprintdot(stdout, mtbdd_a);
    ////////////////////////////////////////////////////////
    
    // ?? proc fce mtbdd_union_cube prepise listy u uz existujiciho mtbdd a je potreba custom fce ??
    // ?? jak funguje custom fce ??

    sylvan_quit();
    lace_stop();
    return 0;
}

/* pozn:
       variables = x1, x2 apod. = seznam vsech jmen vnitrnich uzlu
       symbol sekv. 0/1/2 = pod jakyma podstromama bude uzel (low/high/oba)
       fce cube vytvori mtbdd a da list tam kam ma dle zadani
*/
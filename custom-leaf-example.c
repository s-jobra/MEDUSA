#include <sylvan.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t MY_CUSTOM_LEAF_TYPE_ID = 0;  // Sylvan will assign ID for our custom leaf type

// Will represent the content of our leaf
struct Vec_2D {
    int x;
    int y;
};

/**
 * Handle called when a new leaf is created.
 */
void create_my_leaf(uint64_t* leaf_data_ptr_raw)
{
    // leaf_data_ptr is a pointer to what we store in the leaf - as leaf can store uint64_t, we store there pointers to Vec_2D
    struct Vec_2D** leaf_data_ptr = (struct Vec_2D**) leaf_data_ptr_raw;

    struct Vec_2D* original_leaf_data = *leaf_data_ptr;  
    struct Vec_2D* new_leaf_data      = (struct Vec_2D*) malloc(sizeof(struct Vec_2D));
    
    // Initialize data
    new_leaf_data->x = original_leaf_data->x;
    new_leaf_data->y = original_leaf_data->y;

    *leaf_data_ptr = new_leaf_data;  // Write new leaf data for the leaf freshly created by Sylvan
}

/**
 * Handle called when a new leaf is destroyed.
 */
void destroy_my_leaf(uint64_t leaf_data)
{
    struct Vec_2D* my_leaf_data = (struct Vec_2D*) leaf_data;
    free(my_leaf_data);
}

/**
 * Handle called when two leaves are compared, e.g., when searching in an internal hashtable.
 */
int are_two_leaves_equal(uint64_t a_ptr, uint64_t b_ptr)
{
    struct Vec_2D* leaf_data_a = (struct Vec_2D*) a_ptr;
    struct Vec_2D* leaf_data_b = (struct Vec_2D*) b_ptr;

    return (leaf_data_a->x == leaf_data_b->x) && (leaf_data_a->y == leaf_data_b->y);
}

/**
 * Hashing function used to lookup hastable slots for our leaf.
 */
uint64_t compute_my_leaf_hash(const uint64_t leaf_data_raw, const uint64_t seed)
{
    struct Vec_2D* leaf_data = (struct Vec_2D*) leaf_data_raw;
    return seed * ( ((uint64_t)leaf_data->x) << 32 + (uint64_t)leaf_data->y);
}

/**
 * Converts our leaf to a string for use, e.g., in mtbdd_fprintdot
 */
char* my_leaf_to_str(int comp, uint64_t leaf_data_raw, char* sylvan_buffer, size_t sylvan_buffer_length)
{
    (void) comp;
    struct Vec_2D* leaf_data = (struct Vec_2D*) leaf_data_raw;

    char leaf_str_buffer[100] = {0};
    uint64_t chars_written = snprintf(leaf_str_buffer, 100, "(%d, %d)", leaf_data->x, leaf_data->y);

    // Is the buffer provided by sylvan big enough to hold our point?
    if (chars_written < sylvan_buffer_length) {
        memcpy(sylvan_buffer, leaf_str_buffer, sizeof(char) * chars_written);
        sylvan_buffer[chars_written] = '\0';
        return sylvan_buffer;
    } else {
        // Otherwise we have to return heap allocated buffer (stack allocated would decay)
        char *new_buf = (char*) malloc(sizeof(char) * chars_written);
        memcpy(new_buf, leaf_str_buffer, sizeof(char) * chars_written);
        new_buf[chars_written] = '\0';
        return new_buf;
    }
}

uint32_t register_custom_mtbdd_leaf() {
    uint32_t custom_mtbdd_leaf_type_id = sylvan_mt_create_type();

    sylvan_mt_set_hash(custom_mtbdd_leaf_type_id, compute_my_leaf_hash);
    sylvan_mt_set_equals(custom_mtbdd_leaf_type_id, are_two_leaves_equal);
    sylvan_mt_set_create(custom_mtbdd_leaf_type_id, create_my_leaf);
    sylvan_mt_set_destroy(custom_mtbdd_leaf_type_id, destroy_my_leaf);
    sylvan_mt_set_to_str(custom_mtbdd_leaf_type_id, my_leaf_to_str);

    return custom_mtbdd_leaf_type_id;
}

void init_sylvan() {
    lace_start(1, 0);  // Start lace with 1 thread and <default> size of task queue
	sylvan_set_limits(500LL*1024*1024, 3, 5); // Allocate 100MB
    sylvan_init_package();
    sylvan_init_mtbdd();
}

// Declare an operation called `take_leaf_with_max_euclidian_distance` that accepts pointers to two MTBDDs and a single parameter of type uint64_t
TASK_DECL_3(MTBDD, take_leaf_with_max_euclidian_distance, MTBDD*, MTBDD*, uint64_t);

// Implement `my_leaf_operation`
TASK_IMPL_3(MTBDD, take_leaf_with_max_euclidian_distance, MTBDD*, left_mtbdd_ptr, MTBDD*, right_mtbdd_ptr, uint64_t, param) {
    MTBDD left_mtbdd  = *left_mtbdd_ptr;
    MTBDD right_mtbdd = *right_mtbdd_ptr;
    
    // mtbdd_false denotes that the partial function represented by the (root) MTBDD has no value for the current Boolean variable assignment (path in MTBDD)
    if (left_mtbdd == mtbdd_false) return right_mtbdd;
    if (right_mtbdd == mtbdd_false) return left_mtbdd;
     
    if (mtbdd_isleaf(left_mtbdd) && mtbdd_isleaf(right_mtbdd)) {
        struct Vec_2D* left_data = (struct Vec_2D*) mtbdd_getvalue(left_mtbdd);
        struct Vec_2D* right_data = (struct Vec_2D*) mtbdd_getvalue(right_mtbdd);

        long int left_distance  = (left_data->x * left_data->x) + (left_data->y * left_data->y);
        long int right_distance = (right_data->x * right_data->x) + (right_data->y * right_data->y);

        // We don't have to take sqrt to tell which one is greater
        if (left_distance > right_distance)
            return left_mtbdd;
        else
            return right_mtbdd;
    }
    return mtbdd_invalid;  //  Don't know how to take maximum of the given MTBDDs - they are not leaves. Therefore, Sylvan should recurse deeper.
}


int main() {
    init_sylvan();

    // Setup lace (work-stealing used by sylvan)
    unsigned int mtbdd_worker_count = 1;
    size_t task_deque_size = 0; // Use default task queue size 

    uint32_t custom_mtbdd_leaf_type_id = register_custom_mtbdd_leaf();
    
    BDDSET variables = mtbdd_set_empty();
    variables = mtbdd_set_add(variables, 1);
    variables = mtbdd_set_add(variables, 2);

    struct Vec_2D point_a = {.x = 10, .y = 2}; 
    uint8_t point_a_symbol[] = {1, 1};
    MTBDD leaf_a  = mtbdd_makeleaf(custom_mtbdd_leaf_type_id, (uint64_t) &point_a);
    MTBDD mtbdd_a = mtbdd_cube(variables, point_a_symbol, leaf_a);

    struct Vec_2D point_b = {.x = 11, .y = 2}; 
    uint8_t point_b_symbol[] = {1, 0};
    MTBDD leaf_b  = mtbdd_makeleaf(custom_mtbdd_leaf_type_id, (uint64_t) &point_b);
    MTBDD mtbdd_b = mtbdd_cube(variables, point_b_symbol, leaf_b);
    
    const uint64_t take_leaf_with_max_euclidian_distance_operation_id = (13ul << 34);
    const uint64_t param_value = 0;

    MTBDD result = mtbdd_applyp(mtbdd_a, mtbdd_b, param_value, TASK(take_leaf_with_max_euclidian_distance), take_leaf_with_max_euclidian_distance_operation_id); 
    
    mtbdd_fprintdot(stdout, result);

	sylvan_quit();
	lace_stop();
    return 0;
}

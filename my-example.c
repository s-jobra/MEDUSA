#include "custom_mtbdd.h"
#include "gates.h"

/*
 * ULTIMATE TODO:
 * ---------------
 * hash funkce pro int64_t
 * test hradla
 * predelat print
 *
 * valgrind
 * redo makefile
 * podivat se na testy
 * 
 * resolve vsechny todo a ??
 */

extern uint32_t ltype_id;

/**
 * Function for sylvan and lace setup.
 */
void init_sylvan() {
    lace_start(1, 0); // 1 thread, default task queue size
    sylvan_set_limits(500LL*1024*1024, 3, 5); // Allocate 100MB
    sylvan_init_package();
    sylvan_init_mtbdd();
}

int main()
{
    init_sylvan();
    init_my_leaf();

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

    fclose(f_orig);
    fclose(f_new);
    fclose(f_new2);
    //////////////////////////////////////////////////////// test_end

    sylvan_quit();
    lace_stop();
    return 0;
}
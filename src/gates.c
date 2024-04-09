#include "gates.h"
#include "sylvan_int.h" // Because of 'mtbddnode_t'

prob_t measure(MTBDD *a, uint32_t xt, char *curr_state, int n)
{
    uint32_t var_t;
    if (mtbdd_isleaf(*a)) {
        var_t = n;
    }
    else {
        var_t = mtbdd_getvar(*a);
    }
    long skip_coef;
    if (var_t == 0) {
        skip_coef = 1;
    }
    else {
        skip_coef = get_coef(0, var_t, xt, curr_state);
    }
    prob_t prob = my_mtbdd_prob_sum(*a, xt, curr_state, n);
    prob *= skip_coef;
    return prob;
}

// Decl in header file as this implementation is used by symbolic as well
// can use regular uapply (simpler) because it doesn't create any new nodes
TASK_IMPL_2(MTBDD, _gate_x, MTBDD, t, uint64_t, xt)
{
    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    // Change high and low successors if node variable is the target qubit
    if (mtbdd_isnode(t)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        if (mtbdd_getvar(t) == xt) { 
            return mtbdd_makenode(xt, mtbdd_gethigh(t), mtbdd_getlow(t));
        }
    }
    else { // is a leaf
        return t;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_3(MTBDD, mtbdd_apply_gate, MTBDD, dd, mtbdd_apply_gate_op, op, uint32_t, xt)
{
    sylvan_gc_test();
    sylvan_stats_count(MTBDD_UAPPLY);

    // Check cache
    MTBDD result;
    if (cache_get3(mtbdd_apply_gate_id, dd, (size_t)op, xt, &result)) {
        sylvan_stats_count(MTBDD_UAPPLY_CACHED);
        return result;
    }

    // Every gate on 0 returns 0
    if (dd == mtbdd_false) {
        return mtbdd_false;
    }

    mtbddnode_t n = MTBDD_GETNODE(dd);
    MTBDD target_dd = dd;
    // Add the missing node if needed
    if (mtbddnode_isleaf(n) || xt < mtbddnode_getvariable(n)) {
        target_dd = _mtbdd_makenode(xt, dd, dd);
    }

    // Check if not xt
    result = WRAP(op, target_dd, xt);
    if (result != mtbdd_invalid) {
        if (cache_put3(mtbdd_apply_gate_id, dd, (size_t)op, xt, result)) {
            sylvan_stats_count(MTBDD_UAPPLY_CACHEDPUT);
        }
        return result;
    }
    // Else recursion
    MTBDD low = node_getlow(dd, n);
    MTBDD high = node_gethigh(dd, n);

    mtbdd_refs_spawn(SPAWN(mtbdd_apply_gate, high, op, xt));
    MTBDD new_low = mtbdd_refs_push(CALL(mtbdd_apply_gate, low, op, xt));
    MTBDD new_high = mtbdd_refs_sync(SYNC(mtbdd_apply_gate));
    mtbdd_refs_pop(1);
    result = mtbdd_makenode(mtbddnode_getvariable(n), new_low, new_high);

    if (cache_put3(mtbdd_apply_gate_id, dd, (size_t)op, xt, result)) {
        sylvan_stats_count(MTBDD_UAPPLY_CACHEDPUT);
    }

    return result;
}

/**
 * Permutation based implementation of the Y gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, _gate_y, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_y, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // Change high and low successors and negate the low successor
        MTBDD updated = mtbdd_makenode(xt, my_mtbdd_neg(high), low);
        // Perform the rotations
        return my_mtbdd_coef_rot2(updated);
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Permutation based implementation of the Z gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, _gate_z, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_z, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // Negate the high successor
        return mtbdd_makenode(xt, low, my_mtbdd_neg(high));
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Permutation based implementation of the S gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, _gate_s, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_s, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // Multiply the high successor by i
        return mtbdd_makenode(xt, low, my_mtbdd_coef_rot2(high));
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Permutation based implementation of the T gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, _gate_t, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_t, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // Multiply the high successor by e^(i*pi/4)
        return mtbdd_makenode(xt, low, my_mtbdd_coef_rot1(high));
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Permutation based implementation of the Hadamard gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, _gate_h, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_h, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // low = low + high, high = low - high
        if (low == high) {
            return mtbdd_makenode(xt, my_mtbdd_times_c(low, 2), mtbdd_false);
        }
        else {
            return mtbdd_makenode(xt, my_mtbdd_plus(low, high), my_mtbdd_minus(low, high));
        }
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Permutation based implementation of the Rx(π/2) gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, _gate_rx_pihalf, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_rx_pihalf, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);
        MTBDD rot_low, rot_high;

        // low = low - i * high, high = -i * low + high
        rot_low = my_mtbdd_coef_rot2(low);
        if (low == high) {
            return my_mtbdd_minus(low, rot_low); // new low and high of xt are always the same
        }
        else {
            rot_high = my_mtbdd_coef_rot2(high);
            return mtbdd_makenode(xt, my_mtbdd_minus(low, rot_high), my_mtbdd_minus(high, rot_low));
        }
    }

    return mtbdd_invalid; // Recurse deeper
}

/**
 * Permutation based implementation of the Ry(π/2) gate on the given MTBDD.
 */
TASK_DECL_2(MTBDD, _gate_ry_pihalf, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_ry_pihalf, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // low = low - high, high = low + high
        if (low == high) {
            return mtbdd_makenode(xt, mtbdd_false, my_mtbdd_times_c(low, 2));
        }
        else {
            return mtbdd_makenode(xt, my_mtbdd_minus(low, high), my_mtbdd_plus(low, high));
        }
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_x(MTBDD *p_t, uint32_t xt)
{
    // Check if xt is a missing root not needed -> swap is identical with the original MTBDD
    *p_t = mtbdd_uapply(*p_t, TASK(_gate_x), xt);
}

void gate_y(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_y), xt);
}

void gate_z(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_z), xt);
}

void gate_s(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_s), xt);
}

void gate_t(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_t), xt);
}

void gate_h(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_h), xt );
    mpz_add_ui(c_k, c_k, 1);
}

void gate_rx_pihalf(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_rx_pihalf), xt);
    mpz_add_ui(c_k, c_k, 1);
}

void gate_ry_pihalf(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_ry_pihalf), xt);
    mpz_add_ui(c_k, c_k, 1);
}

//FIXME: change Bxt, Bxt_c create to Bxt * T, Bxt_c * T operations
//FIXME: remove unnecessary applies (bracket multiplication)

void gate_cnot(MTBDD *p_t, uint32_t xt, uint32_t xc)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xc_comp = create_b_xt_comp(xc);
    mtbdd_protect(&b_xc_comp);
    res = my_mtbdd_times(b_xc_comp, t); // Bxc_c * T
    mtbdd_protect(&res);
    mtbdd_unprotect(&b_xc_comp);

    MTBDD b_xc = create_b_xt(xc);
    mtbdd_protect(&b_xc);
    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    MTBDD t_xt = create_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    MTBDD inter_res = my_mtbdd_times(b_xc, b_xt_comp); // Bxc * Bxt_c
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&b_xt_comp);
    inter_res = my_mtbdd_times(inter_res, t_xt); // (Bxc * Bxt_c) * Txt
    mtbdd_unprotect(&t_xt);
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T) + (Bxc * Bxt_c * Txt)

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD t_xt_comp = create_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    mtbdd_unprotect(&t);
    inter_res = my_mtbdd_times(b_xc, b_xt); // Bxc * Bxt
    mtbdd_unprotect(&b_xc);
    mtbdd_unprotect(&b_xt);
    inter_res = my_mtbdd_times(inter_res, t_xt_comp); // (Bxc * Bxt) * Txt_c
    mtbdd_unprotect(&t_xt_comp);
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T + Bxc * Bxt_c * Txt) + (Bxc * Bxt * Txt_c)
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_cz(MTBDD *p_t, uint32_t xt, uint32_t xc)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xc_comp = create_b_xt_comp(xc);
    mtbdd_protect(&b_xc_comp);
    res = my_mtbdd_times(b_xc_comp, t); // Bxc_c * T
    mtbdd_protect(&res);

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    MTBDD inter_res = my_mtbdd_times(b_xt_comp, t); // Bxt_c * T
    mtbdd_protect(&inter_res);
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T) + (Bxt_c * T)

    inter_res = my_mtbdd_times(b_xc_comp, b_xt_comp); // Bxc_c * Bxt_c
    mtbdd_unprotect(&b_xc_comp);
    mtbdd_unprotect(&b_xt_comp);
    inter_res = my_mtbdd_times(inter_res, t); // (Bxc_c * Bxt_c) * T
    res = my_mtbdd_minus(res, inter_res); // (Bxc_c * T + Bxt_c * T) - (Bxc_c * Bxt_c * T)

    MTBDD b_xc = create_b_xt(xc);
    mtbdd_protect(&b_xc);
    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    inter_res = my_mtbdd_times(b_xc, b_xt); // Bxc * Bxt
    mtbdd_unprotect(&b_xc);
    mtbdd_unprotect(&b_xt);
    inter_res = my_mtbdd_times(inter_res, t); // (Bxc * Bxt) * T
    mtbdd_unprotect(&t);
    res = my_mtbdd_minus(res, inter_res); // (Bxc_c * T + Bxt_c * T - Bxc_c * Bxt_c * T) - (Bxc * Bxt * T)
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_toffoli(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xc1_comp = create_b_xt_comp(xc1);
    mtbdd_protect(&b_xc1_comp);
    res = my_mtbdd_times(b_xc1_comp, t); // Bxc_c * T
    mtbdd_protect(&res);

    MTBDD b_xc2_comp = create_b_xt_comp(xc2);
    mtbdd_protect(&b_xc2_comp);
    MTBDD inter_res = my_mtbdd_times(b_xc2_comp, t); // Bxc'_c * T
    mtbdd_protect(&inter_res);
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T) + (Bxc'_c * T)

    inter_res = my_mtbdd_times(b_xc1_comp, b_xc2_comp); // Bxc_c * Bxc'_c
    mtbdd_unprotect(&b_xc1_comp);
    mtbdd_unprotect(&b_xc2_comp);
    inter_res = my_mtbdd_times(inter_res, t); // (Bxc_c * Bxc'_c) * T
    res = my_mtbdd_minus(res, inter_res); // (Bxc_c * T + Bxc'_c * T) - (Bxc_c * Bxc'_c * T)

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD b_xc1 = create_b_xt(xc1);
    mtbdd_protect(&b_xc1);
    MTBDD b_xc2 = create_b_xt(xc2);
    mtbdd_protect(&b_xc2);
    MTBDD t_xt_comp = create_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    inter_res = my_mtbdd_times(b_xt, b_xc1); // Bxt * Bxc
    mtbdd_unprotect(&b_xt);
    inter_res = my_mtbdd_times(inter_res, b_xc2); // (Bxt * Bxc) * Bxc'
    inter_res = my_mtbdd_times(inter_res, t_xt_comp); // (Bxt * Bxc * Bxc') * Txt_c
    mtbdd_unprotect(&t_xt_comp);
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T + Bxc'_c * T - Bxc_c * Bxc'_c * T) + (Bxt * Bxc * Bxc' * Txt_c)

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    MTBDD t_xt = create_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    mtbdd_unprotect(&t);
    inter_res = my_mtbdd_times(b_xt_comp, b_xc1); // Bxt_c * Bxc
    mtbdd_unprotect(&b_xt_comp);
    mtbdd_unprotect(&b_xc1);
    inter_res = my_mtbdd_times(inter_res, b_xc2); // (Bxt_c * Bxc) * Bxc'
    mtbdd_unprotect(&b_xc2);
    inter_res = my_mtbdd_times(inter_res, t_xt); // (Bxt_c * Bxc * Bxc') * Txt
    mtbdd_unprotect(&t_xt);
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T + Bxc'_c * T - Bxc_c * Bxc'_c * T + Bxt * Bxc * Bxc' * Txt_c) \
                                            + (Bxt_c * Bxc * Bxc' * Txt)
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_fredkin(MTBDD *p_t, uint32_t xt1, uint32_t xt2, uint32_t xc)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xc_comp = create_b_xt_comp(xc);
    mtbdd_protect(&b_xc_comp);
    res = my_mtbdd_times(b_xc_comp, t); // Bxc_c * T
    mtbdd_protect(&res);
    mtbdd_unprotect(&b_xc_comp);

    MTBDD b_xt1 = create_b_xt(xt1);
    mtbdd_protect(&b_xt1);
    MTBDD b_xt2 = create_b_xt(xt2);
    mtbdd_protect(&b_xt2);
    MTBDD b_xc = create_b_xt(xc);
    mtbdd_protect(&b_xc);
    MTBDD inter_res = my_mtbdd_times(b_xt1, b_xt2); // Bxt * Bxt'
    mtbdd_protect(&inter_res);
    inter_res = my_mtbdd_times(inter_res, b_xc); // (Bxt * Bxt') * Bxc
    inter_res = my_mtbdd_times(inter_res, t); // (Bxt * Bxt' * Bxc) * T
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T) + (Bxt * Bxt' * Bxc * T)

    MTBDD b_xt1_comp = create_b_xt_comp(xt1);
    mtbdd_protect(&b_xt1_comp);
    MTBDD b_xt2_comp = create_b_xt_comp(xt2);
    mtbdd_protect(&b_xt2_comp);
    inter_res = my_mtbdd_times(b_xt1_comp, b_xt2_comp); // Bxt_c * Bxt'_c
    inter_res = my_mtbdd_times(inter_res, b_xc); // (Bxt_c * Bxt'_c) * Bxc
    inter_res = my_mtbdd_times(inter_res, t); // (Bxt_c * Bxt'_c * Bxc) * T
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T + Bxt * Bxt' * Bxc * T) + (Bxt_c * Bxt'_c * Bxc * T)

    MTBDD t_xt1_comp = create_t_xt_comp(t, xt1);
    mtbdd_protect(&t_xt1_comp);
    MTBDD t_xt1_comp_xt2 = create_t_xt(t_xt1_comp, xt2);
    mtbdd_protect(&t_xt1_comp_xt2);
    mtbdd_unprotect(&t_xt1_comp);
    inter_res = my_mtbdd_times(b_xt1, b_xt2_comp); // Bxt * Bxt'_c
    mtbdd_unprotect(&b_xt1);
    mtbdd_unprotect(&b_xt2_comp);
    inter_res = my_mtbdd_times(inter_res, b_xc); // (Bxt * Bxt'_c) * Bxc
    inter_res = my_mtbdd_times(inter_res, t_xt1_comp_xt2); // (Bxt * Bxt'_c * Bxc) * T(xt_c)xt'
    mtbdd_unprotect(&t_xt1_comp_xt2);
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T + Bxt * Bxt' * Bxc * T + Bxt_c * Bxt'_c * Bxc * T) \
                                            + (Bxt * Bxt'_c * Bxc * T(xt_c)xt')

    MTBDD t_xt1 = create_t_xt(t, xt1);
    mtbdd_protect(&t_xt1);
    mtbdd_unprotect(&t);
    MTBDD t_xt1_xt2_comp = create_t_xt_comp(t_xt1, xt2);
    mtbdd_protect(&t_xt1_xt2_comp);
    mtbdd_unprotect(&t_xt1);
    inter_res = my_mtbdd_times(b_xt1_comp, b_xt2); // Bxt_c * Bxt'
    mtbdd_unprotect(&b_xt1_comp);
    mtbdd_unprotect(&b_xt2);
    inter_res = my_mtbdd_times(inter_res, b_xc); // (Bxt_c * Bxt') * Bxc
    mtbdd_unprotect(&b_xc);
    inter_res = my_mtbdd_times(inter_res, t_xt1_xt2_comp); // (Bxt_c * Bxt' * Bxc) * T(xt)xt'_c
    mtbdd_unprotect(&t_xt1_xt2_comp);
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T + Bxt * Bxt' * Bxc * T + Bxt_c * Bxt'_c * Bxc * T \
                                             + Bxt * Bxt'_c * Bxc * T(xt_c)xt') \
                                          + (Bxt_c * Bxt' * Bxc * T(xt)xt'_c)
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_mcx(MTBDD *p_t, qparam_list_t *qparams)
{
    // Assumes indices in qparams are in the reverse order of the input file! (target qubit first, qc1 last)

    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    qparam_list_first(qparams);

    // Target qubit part:
    MTBDD t_xt = create_t_xt(t, qparams->active->q_index);
    mtbdd_protect(&t_xt);
    MTBDD b_xn_comp = create_b_xt_comp(qparams->active->q_index);
    mtbdd_protect(&b_xn_comp);
    MTBDD bracket_left = my_mtbdd_times(b_xn_comp, t_xt); // Bxt_c * Txt
    mtbdd_protect(&bracket_left);
    mtbdd_unprotect(&t_xt);

    MTBDD t_xt_comp = create_t_xt_comp(t, qparams->active->q_index);
    mtbdd_protect(&t_xt_comp);
    MTBDD b_xn = create_b_xt(qparams->active->q_index);
    mtbdd_protect(&b_xn);
    MTBDD bracket_right = my_mtbdd_times(b_xn, t_xt_comp); // Bxt * Txt_c
    mtbdd_protect(&bracket_right);
    mtbdd_unprotect(&t_xt_comp);

    res = my_mtbdd_plus(bracket_left, bracket_right); // (Bxt_c * Txt) + (Bxt * Txt_c)
    mtbdd_protect(&res);

    // Get the last control qubit
    qparam_list_next(qparams);

    // Handling the control qubits
    while (qparams->active) {
        b_xn = create_b_xt(qparams->active->q_index);
        bracket_right = my_mtbdd_times(b_xn, res);             // BxcN * (result so far)
        b_xn_comp = create_b_xt_comp(qparams->active->q_index);
        bracket_left = my_mtbdd_times(b_xn_comp, t);           // BxcN_c * T
        res = my_mtbdd_plus(bracket_left, bracket_right);      // (BxcN_c * T) + (BxcN * (result so far))
        qparam_list_next(qparams);  // move to the next control qubit
    }
    mtbdd_unprotect(&b_xn);
    mtbdd_unprotect(&b_xn_comp);
    mtbdd_unprotect(&t);
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);

    *p_t = res;
    mtbdd_unprotect(&res);
}
/* end of "gates.c" */
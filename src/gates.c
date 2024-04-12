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
    MTBDD low = node_getlow(dd, n); // can use dd as if the target dd is different, it means xt is the root
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

TASK_IMPL_5(MTBDD, mtbdd_apply_cgate, MTBDD, dd, mtbdd_apply_gate_op, op, uint32_t, xc, uint32_t, xt, bool, xc_generated)
{
    sylvan_gc_test();
    sylvan_stats_count(MTBDD_UAPPLY);

    // Check cache
    MTBDD result;
    if (cache_get3(mtbdd_apply_cgate_id, dd, xc, xt, &result)) {
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
    if (!xc_generated && (mtbddnode_isleaf(n) || xc < mtbddnode_getvariable(n))) {
        target_dd = _mtbdd_makenode(xc, dd, dd);
    }
    else if (xc_generated && (mtbddnode_isleaf(n) || xt < mtbddnode_getvariable(n))) {
        target_dd = _mtbdd_makenode(xt, dd, dd);
    }

    // Check if not xc
    if (mtbdd_getvar(target_dd) == xc) {
        xc_generated = true;
        MTBDD new_high = mtbdd_refs_push(CALL(mtbdd_apply_cgate, mtbdd_gethigh(target_dd), op, xc, xt, xc_generated));
        mtbdd_refs_pop(1);
        result = mtbdd_makenode(xc, mtbdd_getlow(target_dd), new_high);

        if (cache_put3(mtbdd_apply_cgate_id, dd, xc, xt, result)) {
            sylvan_stats_count(MTBDD_UAPPLY_CACHEDPUT);
        }
        return result;
    }
    // Else check if not xt
    result = WRAP(op, target_dd, xt);
    if (result != mtbdd_invalid) {
        if (cache_put3(mtbdd_apply_gate_id, dd, xc, xt, result)) {
            sylvan_stats_count(MTBDD_UAPPLY_CACHEDPUT);
        }
        return result;
    }
    // Else recursion (both successors)
    MTBDD low = node_getlow(dd, n); // can use dd and n as if the target dd is different, it means xc/xt is the root
    MTBDD high = node_gethigh(dd, n);

    mtbdd_refs_spawn(SPAWN(mtbdd_apply_cgate, high, op, xc, xt, xc_generated));
    MTBDD new_low = mtbdd_refs_push(CALL(mtbdd_apply_cgate, low, op, xc, xt, xc_generated));
    MTBDD new_high = mtbdd_refs_sync(SYNC(mtbdd_apply_cgate));
    mtbdd_refs_pop(1);
    result = mtbdd_makenode(mtbddnode_getvariable(n), new_low, new_high);

    if (cache_put3(mtbdd_apply_cgate_id, dd, xc, xt, result)) {
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

void gate_cnot(MTBDD *p_t, uint32_t xt, uint32_t xc)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = my_mtbdd_b_xt_comp_mul(t, xc); // Bxc_c * T
    mtbdd_protect(&res);

    MTBDD t_xt = my_mtbdd_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    MTBDD bracket_left = my_mtbdd_b_xt_comp_mul(t_xt, xt); // Bxt_c * Txt
    mtbdd_protect(&bracket_left);
    mtbdd_unprotect(&t_xt);

    MTBDD t_xt_comp = my_mtbdd_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    mtbdd_unprotect(&t);
    MTBDD bracket_right = my_mtbdd_b_xt_mul(t_xt_comp, xt); // Bxt * Txt_c
    mtbdd_protect(&bracket_right);
    mtbdd_unprotect(&t_xt_comp);

    MTBDD inter_res = my_mtbdd_plus(bracket_left, bracket_right); // (Bxt_c * Txt) + (Bxt * Txt_c)
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);
    inter_res = my_mtbdd_b_xt_mul(inter_res, xc); // Bxc * (Bxt_c * Txt + Bxt * Txt_c)

    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T) + (Bxc * (Bxt_c * Txt + Bxt * Txt_c))
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_cz(MTBDD *p_t, uint32_t xt, uint32_t xc)
{
    *p_t = my_mtbdd_apply_cgate(*p_t, TASK(_gate_z), xc, xt);
}

void gate_toffoli(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = my_mtbdd_b_xt_comp_mul(t, xc1); // Bxc_c * T
    mtbdd_protect(&res);

    MTBDD t_xt = my_mtbdd_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    MTBDD bracket_left = my_mtbdd_b_xt_comp_mul(t_xt, xt); // Bxt_c * Txt
    mtbdd_protect(&bracket_left);
    mtbdd_unprotect(&t_xt);

    MTBDD t_xt_comp = my_mtbdd_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    MTBDD bracket_right = my_mtbdd_b_xt_mul(t_xt_comp, xt); // Bxt * Txt_c
    mtbdd_protect(&bracket_right);
    mtbdd_unprotect(&t_xt_comp);

    MTBDD inter_res = my_mtbdd_plus(bracket_left, bracket_right); // (Bxt_c * Txt) + (Bxt * Txt_c)
    mtbdd_protect(&inter_res);

    bracket_right = my_mtbdd_b_xt_mul(inter_res, xc2); // Bxc' * (Bxt_c * Txt + Bxt * Txt_c)
    bracket_left = my_mtbdd_b_xt_comp_mul(t, xc2); // Bxc'_c * T
    mtbdd_unprotect(&t);
    
    inter_res = my_mtbdd_plus(bracket_left, bracket_right); // (Bxc'_c * T) + (Bxc' * (Bxt_c * Txt + Bxt * Txt_c))
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);

    inter_res = my_mtbdd_b_xt_mul(inter_res, xc1); // Bxc * (Bxc'_c * T + Bxc' * (Bxt_c * Txt + Bxt * Txt_c))
    res = my_mtbdd_plus(res, inter_res); // (Bxc_c * T) + (Bxc * (Bxc'_c * T + Bxc' * (Bxt_c * Txt + Bxt * Txt_c)))
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

    // Target qubit part
    MTBDD t_xt = my_mtbdd_t_xt(t, qparams->active->q_index);
    mtbdd_protect(&t_xt);
    MTBDD bracket_left = my_mtbdd_b_xt_comp_mul(t_xt, qparams->active->q_index); // Bxt_c * Txt
    mtbdd_protect(&bracket_left);
    mtbdd_unprotect(&t_xt);

    MTBDD t_xt_comp = my_mtbdd_t_xt_comp(t, qparams->active->q_index);
    mtbdd_protect(&t_xt_comp);
    MTBDD bracket_right = my_mtbdd_b_xt_mul(t_xt_comp, qparams->active->q_index); // Bxt * Txt_c
    mtbdd_protect(&bracket_right);
    mtbdd_unprotect(&t_xt_comp);

    res = my_mtbdd_plus(bracket_left, bracket_right); // (Bxt_c * Txt) + (Bxt * Txt_c)
    mtbdd_protect(&res);

    // Get the last control qubit
    qparam_list_next(qparams);

    // Handling the control qubits
    while (qparams->active) {
        bracket_right = my_mtbdd_b_xt_mul(res, qparams->active->q_index);   // BxcN * (result so far)
        bracket_left = my_mtbdd_b_xt_comp_mul(t, qparams->active->q_index); // BxcN_c * T
        res = my_mtbdd_plus(bracket_left, bracket_right);                   // (BxcN_c * T) + (BxcN * (result so far))
        qparam_list_next(qparams);  // move to the next control qubit
    }
    mtbdd_unprotect(&t);
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);

    *p_t = res;
    mtbdd_unprotect(&res);
}
/* end of "gates.c" */
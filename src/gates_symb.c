#include "gates_symb.h"

void gate_symb_x(MTBDD *p_t, uint32_t xt)
{
    // uses the same implementation as the regular MTBDDs
    *p_t = mtbdd_uapply(*p_t, TASK(_gate_x), xt);
}


TASK_DECL_2(MTBDD, _gate_symb_y, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_symb_y, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // Change high and low successors and negate the low successor
        MTBDD updated = mtbdd_makenode(xt, my_mtbdd_symb_neg(high), low);
        // Perform the rotations
        return my_mtbdd_symb_coef_rot2(updated);
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_y(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_symb_y), xt);
}


TASK_DECL_2(MTBDD, _gate_symb_z, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_symb_z, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // Negate the high successor
        return mtbdd_makenode(xt, low, my_mtbdd_symb_neg(high));
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_z(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_symb_z), xt);
}


TASK_DECL_2(MTBDD, _gate_symb_s, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_symb_s, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // Multiply the high successor by i
        return mtbdd_makenode(xt, low, my_mtbdd_symb_coef_rot2(high));
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_s(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_symb_s), xt);
}


TASK_DECL_2(MTBDD, _gate_symb_t, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_symb_t, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // Multiply the high successor by e^(i*pi/4)
        return mtbdd_makenode(xt, low, my_mtbdd_symb_coef_rot1(high));
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_t(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_symb_t), xt);
}


TASK_DECL_2(MTBDD, _gate_symb_h, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_symb_h, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // low = low + high, high = low - high
        if (low == high) {
            return mtbdd_makenode(xt, my_mtbdd_symb_times_c(low, 2), mtbdd_false);
        }
        else {
            return mtbdd_makenode(xt, my_mtbdd_symb_plus(low, high), my_mtbdd_symb_minus(low, high));
        }
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_h(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_symb_h), xt);
    mpz_add_ui(cs_k, cs_k, 1);
}


TASK_DECL_2(MTBDD, _gate_symb_rx_pihalf, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_symb_rx_pihalf, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);
        MTBDD rot_low, rot_high;

        // low = low - i * high, high = -i * low + high
        rot_low = my_mtbdd_symb_coef_rot2(low);
        if (low == high) {
            return my_mtbdd_symb_minus(low, rot_low); // new low and high of xt are always the same
        }
        else {
            rot_high = my_mtbdd_symb_coef_rot2(high);
            return mtbdd_makenode(xt, my_mtbdd_symb_minus(low, rot_high), my_mtbdd_symb_minus(high, rot_low));
        }
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_rx_pihalf(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_symb_rx_pihalf), xt);
    mpz_add_ui(cs_k, cs_k, 1);
}


TASK_DECL_2(MTBDD, _gate_symb_ry_pihalf, MTBDD, uint32_t);
TASK_IMPL_2(MTBDD, _gate_symb_ry_pihalf, MTBDD, t, uint32_t, xt)
{
    // t has to be a nonterminal because of the apply
    uint32_t var = mtbdd_getvar(t);

    if (var == xt) {
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        // low = low - high, high = low + high
        if (low == high) {
            return mtbdd_makenode(xt, mtbdd_false, my_mtbdd_symb_times_c(low, 2));
        }
        else {
            return mtbdd_makenode(xt, my_mtbdd_symb_minus(low, high), my_mtbdd_symb_plus(low, high));
        }
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_ry_pihalf(MTBDD *p_t, uint32_t xt)
{
    *p_t = my_mtbdd_apply_gate(*p_t, TASK(_gate_symb_ry_pihalf), xt);
    mpz_add_ui(cs_k, cs_k, 1);
}

void gate_symb_cnot(MTBDD *p_t, uint32_t xt, uint32_t xc)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = my_mtbdd_symb_b_xt_comp_mul(t, xc); // Bxc_c * T
    mtbdd_protect(&res);

    MTBDD t_xt = my_mtbdd_symb_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    MTBDD bracket_left = my_mtbdd_symb_b_xt_comp_mul(t_xt, xt); // Bxt_c * Txt
    mtbdd_protect(&bracket_left);
    mtbdd_unprotect(&t_xt);

    MTBDD t_xt_comp = my_mtbdd_symb_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    mtbdd_unprotect(&t);
    MTBDD bracket_right = my_mtbdd_symb_b_xt_mul(t_xt_comp, xt); // Bxt * Txt_c
    mtbdd_protect(&bracket_right);
    mtbdd_unprotect(&t_xt_comp);

    MTBDD inter_res = my_mtbdd_symb_plus(bracket_left, bracket_right); // (Bxt_c * Txt) + (Bxt * Txt_c)
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);
    inter_res = my_mtbdd_symb_b_xt_mul(inter_res, xc); // Bxc * (Bxt_c * Txt + Bxt * Txt_c)

    res = my_mtbdd_symb_plus(res, inter_res); // (Bxc_c * T) + (Bxc * (Bxt_c * Txt + Bxt * Txt_c))
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_symb_cz(MTBDD *p_t, uint32_t xt, uint32_t xc)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = my_mtbdd_symb_b_xt_comp_mul(t, xc); // Bxc_c * T
    mtbdd_protect(&res);

    MTBDD bracket_left = my_mtbdd_symb_b_xt_comp_mul(t, xt); // Bxt_c * T
    mtbdd_protect(&bracket_left);

    MTBDD bracket_right = my_mtbdd_symb_b_xt_mul(t, xt); // Bxt * T
    mtbdd_protect(&bracket_right);
    mtbdd_unprotect(&t);

    MTBDD inter_res = my_mtbdd_symb_minus(bracket_left, bracket_right); // (Bxt_c * T) - (Bxt * T)
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);
    inter_res = my_mtbdd_symb_b_xt_mul(inter_res, xc); // Bxc * (Bxt_c * T - Bxt * T)

    res = my_mtbdd_symb_plus(res, inter_res); // (Bxc_c * T) + (Bxc * (Bxt_c * T - Bxt * T))
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_symb_toffoli(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = my_mtbdd_symb_b_xt_comp_mul(t, xc1); // Bxc_c * T
    mtbdd_protect(&res);

    MTBDD t_xt = my_mtbdd_symb_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    MTBDD bracket_left = my_mtbdd_symb_b_xt_comp_mul(t_xt, xt); // Bxt_c * Txt
    mtbdd_protect(&bracket_left);
    mtbdd_unprotect(&t_xt);

    MTBDD t_xt_comp = my_mtbdd_symb_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    MTBDD bracket_right = my_mtbdd_symb_b_xt_mul(t_xt_comp, xt); // Bxt * Txt_c
    mtbdd_protect(&bracket_right);
    mtbdd_unprotect(&t_xt_comp);

    MTBDD inter_res = my_mtbdd_symb_plus(bracket_left, bracket_right); // (Bxt_c * Txt) + (Bxt * Txt_c)
    mtbdd_protect(&inter_res);

    bracket_right = my_mtbdd_symb_b_xt_mul(inter_res, xc2); // Bxc' * (Bxt_c * Txt + Bxt * Txt_c)
    bracket_left = my_mtbdd_symb_b_xt_comp_mul(t, xc2); // Bxc'_c * T
    mtbdd_unprotect(&t);
    
    inter_res = my_mtbdd_symb_plus(bracket_left, bracket_right); // (Bxc'_c * T) + (Bxc' * (Bxt_c * Txt + Bxt * Txt_c))
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);

    inter_res = my_mtbdd_symb_b_xt_mul(inter_res, xc1); // Bxc * (Bxc'_c * T + Bxc' * (Bxt_c * Txt + Bxt * Txt_c))
    res = my_mtbdd_symb_plus(res, inter_res); // (Bxc_c * T) + (Bxc * (Bxc'_c * T + Bxc' * (Bxt_c * Txt + Bxt * Txt_c)))
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_symb_mcx(MTBDD *p_t, qparam_list_t *qparams)
{
    // Assumes indices in qparams are in the reverse order of the input file! (target qubit first, qc1 last)

    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    qparam_list_first(qparams);

    // Target qubit part
    MTBDD t_xt = my_mtbdd_symb_t_xt(t, qparams->active->q_index);
    mtbdd_protect(&t_xt);
    MTBDD bracket_left = my_mtbdd_symb_b_xt_comp_mul(t_xt, qparams->active->q_index); // Bxt_c * Txt
    mtbdd_protect(&bracket_left);
    mtbdd_unprotect(&t_xt);

    MTBDD t_xt_comp = my_mtbdd_symb_t_xt_comp(t, qparams->active->q_index);
    mtbdd_protect(&t_xt_comp);
    MTBDD bracket_right = my_mtbdd_symb_b_xt_mul(t_xt_comp, qparams->active->q_index); // Bxt * Txt_c
    mtbdd_protect(&bracket_right);
    mtbdd_unprotect(&t_xt_comp);

    res = my_mtbdd_symb_plus(bracket_left, bracket_right); // (Bxt_c * Txt) + (Bxt * Txt_c)
    mtbdd_protect(&res);

    // Get the last control qubit
    qparam_list_next(qparams);

    // Handling the control qubits
    while (qparams->active) {
        bracket_right = my_mtbdd_symb_b_xt_mul(res, qparams->active->q_index);   // BxcN * (result so far)
        bracket_left = my_mtbdd_symb_b_xt_comp_mul(t, qparams->active->q_index); // BxcN_c * T
        res = my_mtbdd_symb_plus(bracket_left, bracket_right);                   // (BxcN_c * T) + (BxcN * (result so far))
        qparam_list_next(qparams);  // move to the next control qubit
    }
    mtbdd_unprotect(&t);
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);

    *p_t = res;
    mtbdd_unprotect(&res);
}

/* end of "gates_symb.c" */
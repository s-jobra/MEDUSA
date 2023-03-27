#include "gates.h"

int measure(MTBDD* a, uint32_t xt)
{
    int bit_val = 0;
    MTBDD t_xt = create_t_xt(*a, xt);
    mtbdd_protect(&t_xt);
    //TODO: gmp prob count init
    
    // 1 create Txt
    // 2 call sum all nodes on Txt (one argument will be num_ptr to sum)
    // 3 calculate the probability of |1>
    // 4 generate rand num and return the result with the given probability
    mtbdd_unprotect(&t_xt);
    return bit_val;
}

TASK_IMPL_2(MTBDD, m_gate_x, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    // Change high and low successors if node variable is the target qubit
    if (mtbdd_isnode(a)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        if (mtbdd_getvar(a) == xt) { 
            return mtbdd_makenode(xt, mtbdd_gethigh(a), mtbdd_getlow(a));
        }
    }
    else { // is a leaf
        return a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, m_gate_y, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isnode(a)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        uint32_t var_a = mtbdd_getvar(a);
        MTBDD high = mtbdd_gethigh(a);
        MTBDD low = mtbdd_getlow(a);

        if (var_a == xt) { 
            // Change high and low successors and negate the low successor
            MTBDD updated = mtbdd_makenode(xt, my_mtbdd_neg(high), low);
            // Perform the rotations
            return my_mtbdd_coef_rot2(updated);
        }
        
        //If child's variable is > xt or it is a leaf, the target node has to be generated manually
        if (var_a < xt) {
            MTBDD new_high = mtbdd_false, new_low = mtbdd_false;
            if (mtbdd_isleaf(high) || (mtbdd_getvar(high) > xt)) {
                new_high = mtbdd_makenode(xt, my_mtbdd_neg(high), high);
                new_high = my_mtbdd_coef_rot2(new_high);
                high = new_high;
            }

            if (mtbdd_isleaf(low) || (mtbdd_getvar(low) > xt)) {
                new_low = mtbdd_makenode(xt, my_mtbdd_neg(low), low);
                new_low = my_mtbdd_coef_rot2(new_low);
                low = new_low;
            }

            if (new_high != mtbdd_false || new_low != mtbdd_false) {
                return mtbdd_makenode(var_a, low, high);
            }
        }
    }
    else { // is a leaf
        return a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, m_gate_z, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isnode(a)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        uint32_t var_a = mtbdd_getvar(a);
        MTBDD high = mtbdd_gethigh(a);
        MTBDD low = mtbdd_getlow(a);

        if (var_a == xt) { 
            // Negate the high successor
            return mtbdd_makenode(xt, low, my_mtbdd_neg(high));
        }

        if (var_a < xt) {
            MTBDD new_high = mtbdd_false, new_low = mtbdd_false;
            if (mtbdd_isleaf(high) || (mtbdd_getvar(high) > xt)) {
                new_high = mtbdd_makenode(xt, high, my_mtbdd_neg(high));
                high = new_high;
            }

            if (mtbdd_isleaf(low) || (mtbdd_getvar(low) > xt)) {
                new_low = mtbdd_makenode(xt, low, my_mtbdd_neg(low));
                low = new_low;
            }

            if (new_high != mtbdd_false || new_low != mtbdd_false) {
                return mtbdd_makenode(var_a, low, high);
            }
        }
    }
    else { // is a leaf
        return a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, m_gate_s, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isnode(a)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        uint32_t var_a = mtbdd_getvar(a);
        MTBDD high = mtbdd_gethigh(a);
        MTBDD low = mtbdd_getlow(a);

        if (var_a == xt) { 
            // Multiply the high successor by i
            return mtbdd_makenode(xt, low, my_mtbdd_coef_rot2(high));
        }
        
        if (var_a < xt) {
            MTBDD new_high = mtbdd_false, new_low = mtbdd_false;
            if (mtbdd_isleaf(high) || (mtbdd_getvar(high) > xt)) {
                new_high = mtbdd_makenode(xt, high, my_mtbdd_coef_rot2(high));
                high = new_high;
            }

            if (mtbdd_isleaf(low) || (mtbdd_getvar(low) > xt)) {
                new_low = mtbdd_makenode(xt, low, my_mtbdd_coef_rot2(low));
                low = new_low;
            }

            if (new_high != mtbdd_false || new_low != mtbdd_false) {
                return mtbdd_makenode(var_a, low, high);
            }
        }
    }
    else { // is a leaf
        return a;
    }

    return mtbdd_invalid; // Recurse deeper
}

TASK_IMPL_2(MTBDD, m_gate_t, MTBDD, a, uint64_t, xt)
{
    // Partial function check
    if (a == mtbdd_false) return mtbdd_false;

    if (mtbdd_isnode(a)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        uint32_t var_a = mtbdd_getvar(a);
        MTBDD high = mtbdd_gethigh(a);
        MTBDD low = mtbdd_getlow(a);

        if (var_a == xt) { 
            // Multiply the high successor by e^(i*pi/4)
            return mtbdd_makenode(xt, low, my_mtbdd_coef_rot1(high));
        }

        if (var_a < xt) {
            MTBDD new_high = mtbdd_false, new_low = mtbdd_false;
            if (mtbdd_isleaf(high) || (mtbdd_getvar(high) > xt)) {
                new_high = mtbdd_makenode(xt, high, my_mtbdd_coef_rot1(high));
                high = new_high;
            }

            if (mtbdd_isleaf(low) || (mtbdd_getvar(low) > xt)) {
                new_low = mtbdd_makenode(xt, low, my_mtbdd_coef_rot1(low));
                low = new_low;
            }

            if (new_high != mtbdd_false || new_low != mtbdd_false) {
                return mtbdd_makenode(var_a, low, high);
            }
        }
    }
    else { // is a leaf
        return a;
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_x(MTBDD* a, uint32_t xt)
{
    *a = mtbdd_uapply(*a, TASK(m_gate_x), xt);
}

void gate_y(MTBDD* a, uint32_t xt)
{
    if (*a != mtbdd_false) { // check if xt shouldnt' be root
        if (xt < mtbdd_getvar(*a)) {
            *a = _mtbdd_makenode(xt, *a, *a);
        }
    }
    *a = mtbdd_uapply(*a, TASK(m_gate_y), xt);
}

void gate_z(MTBDD* a, uint32_t xt)
{
    if (*a != mtbdd_false) {  // check if xt shouldnt' be root
        if (xt < mtbdd_getvar(*a)) {
            *a = _mtbdd_makenode(xt, *a, *a);
        }
    }
    *a = mtbdd_uapply(*a, TASK(m_gate_z), xt);
}

void gate_s(MTBDD* a, uint32_t xt)
{
    if (*a != mtbdd_false) {  // check if xt shouldnt' be root
        if (xt < mtbdd_getvar(*a)) {
            *a = _mtbdd_makenode(xt, *a, *a);
        }
    }
    *a = mtbdd_uapply(*a, TASK(m_gate_s), xt);
}

void gate_t(MTBDD* a, uint32_t xt)
{
    if (*a != mtbdd_false) { // check if xt shouldnt' be root
        if (xt < mtbdd_getvar(*a)) {
            *a = _mtbdd_makenode(xt, *a, *a);
        }
    }
    *a = mtbdd_uapply(*a, TASK(m_gate_t), xt);
}

void gate_h(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = create_t_xt_comp(t, xt);  // Txt_c
    mtbdd_protect(&res);

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    MTBDD t_xt = create_t_xt(t,xt);
    mtbdd_protect(&t_xt);

    MTBDD inter_res = my_mtbdd_times(b_xt_comp, t_xt); // Bxt_c * Txt
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&b_xt_comp);
    mtbdd_unprotect(&t_xt);

    res = my_mtbdd_plus(res, inter_res); // Txt_c + (Bxt_c * Txt)

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    inter_res = my_mtbdd_times(b_xt, t); // Bxt * T
    mtbdd_unprotect(&b_xt);
    mtbdd_unprotect(&t);
    res = my_mtbdd_minus(res, inter_res); // (Txt_c + Bxt_c * Txt) - Bxt * T
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);

    mpz_add_ui(c_k, c_k, 1); // (1/√2) * (Txt_c + Bxt_c * Txt - Bxt * T)
}

void gate_rx_pihalf(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD t_xt_comp = create_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    res = my_mtbdd_times(b_xt, t_xt_comp); // Bxt * Txt_c
    mtbdd_protect(&res);
    mtbdd_unprotect(&b_xt);
    mtbdd_unprotect(&t_xt_comp);

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    MTBDD t_xt = create_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    MTBDD inter_res = my_mtbdd_times(b_xt_comp, t_xt); // Bxt_c * Txt
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&b_xt_comp);
    mtbdd_unprotect(&t_xt);
    res = my_mtbdd_plus(res, inter_res); // (Bxt * Txt_c) + (Bxt_c * Txt)
    mtbdd_unprotect(&inter_res);

    res = my_mtbdd_coef_rot2(res); // ω² * (Bxt * Txt_c + Bxt_c * Txt)
    res = my_mtbdd_minus(t, res); // T - (ω² * (Bxt * Txt_c + Bxt_c * Txt))
    mtbdd_unprotect(&t);

    *p_t = res;
    mtbdd_unprotect(&res);

    mpz_add_ui(c_k, c_k, 1); // (1/√2) * (T - ω² * (Bxt * Txt_c + Bxt_c * Txt))
}

void gate_ry_pihalf(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = create_t_xt_comp(t, xt); // Txt_c

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD inter_res = my_mtbdd_times(b_xt, t); // Bxt * T
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&b_xt);
    res = my_mtbdd_plus(res, inter_res); // Txt_c + (Bxt * T)
    mtbdd_protect(&res);

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    MTBDD t_xt = create_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    mtbdd_unprotect(&t);
    inter_res = my_mtbdd_times(b_xt_comp, t_xt); // Bxt_c * Txt
    mtbdd_unprotect(&b_xt_comp);
    mtbdd_unprotect(&t_xt);
    res = my_mtbdd_minus(res, inter_res); // (Txt_c + Bxt * T) - (Bxt_c * Txt)
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);

    mpz_add_ui(c_k, c_k, 1); // (1/√2) * (Txt_c + Bxt * T - Bxt_c * Txt)
}

void gate_cnot(MTBDD* p_t, uint32_t xt, uint32_t xc)
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

void gate_cz(MTBDD* p_t, uint32_t xt, uint32_t xc)
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

void gate_toffoli(MTBDD* p_t, uint32_t xt, uint32_t xc1, uint32_t xc2)
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

void gate_fredkin(MTBDD* p_t, uint32_t xt1, uint32_t xt2, uint32_t xc)
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
/* end of "gates.c" */
#include "gates_symb.h"

void gate_symb_x(MTBDD *p_t, uint32_t xt)
{
    // uses the same implementation as the regular MTBDDs
    *p_t = mtbdd_uapply(*p_t, TASK(m_gate_x), xt);
}


TASK_DECL_2(MTBDD, _gate_symb_y, MTBDD, uint64_t);
TASK_IMPL_2(MTBDD, _gate_symb_y, MTBDD, t, uint64_t, xt)
{
    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    if (mtbdd_isnode(t)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        uint32_t var = mtbdd_getvar(t);
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        if (var == xt) { 
            // Change high and low successors and negate the low successor
            MTBDD updated = mtbdd_makenode(xt, my_mtbdd_symb_neg(high), low);
            // Perform the rotations
            return my_mtbdd_symb_coef_rot2(updated);
        }
        
        //If child's variable is > xt or it is a leaf, the target node has to be generated manually
        if (var < xt) {
            MTBDD new_high = mtbdd_false, new_low = mtbdd_false;
            if (mtbdd_isleaf(high) || (mtbdd_getvar(high) > xt)) {
                new_high = mtbdd_makenode(xt, my_mtbdd_symb_neg(high), high);
                new_high = my_mtbdd_symb_coef_rot2(new_high);
                high = new_high;
            }

            if (mtbdd_isleaf(low) || (mtbdd_getvar(low) > xt)) {
                new_low = mtbdd_makenode(xt, my_mtbdd_symb_neg(low), low);
                new_low = my_mtbdd_symb_coef_rot2(new_low);
                low = new_low;
            }

            if (new_high != mtbdd_false || new_low != mtbdd_false) {
                return mtbdd_makenode(var, low, high);
            }
        }
    }
    else { // is a leaf
        return t;
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_y(MTBDD *p_t, uint32_t xt)
{
    check_xt_root_missing(p_t, xt);
    *p_t = mtbdd_uapply(*p_t, TASK(_gate_symb_y), xt);
}


TASK_DECL_2(MTBDD, _gate_symb_z, MTBDD, uint64_t);
TASK_IMPL_2(MTBDD, _gate_symb_z, MTBDD, t, uint64_t, xt)
{
    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    if (mtbdd_isnode(t)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        uint32_t var = mtbdd_getvar(t);
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        if (var == xt) { 
            // Negate the high successor
            return mtbdd_makenode(xt, low, my_mtbdd_symb_neg(high));
        }

        if (var < xt) {
            MTBDD new_high = mtbdd_false, new_low = mtbdd_false;
            if (mtbdd_isleaf(high) || (mtbdd_getvar(high) > xt)) {
                new_high = mtbdd_makenode(xt, high, my_mtbdd_symb_neg(high));
                high = new_high;
            }

            if (mtbdd_isleaf(low) || (mtbdd_getvar(low) > xt)) {
                new_low = mtbdd_makenode(xt, low, my_mtbdd_symb_neg(low));
                low = new_low;
            }

            if (new_high != mtbdd_false || new_low != mtbdd_false) {
                return mtbdd_makenode(var, low, high);
            }
        }
    }
    else { // is a leaf
        return t;
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_z(MTBDD *p_t, uint32_t xt)
{
    check_xt_root_missing(p_t, xt);
    *p_t = mtbdd_uapply(*p_t, TASK(_gate_symb_z), xt);
}

TASK_DECL_2(MTBDD, _gate_symb_s, MTBDD, uint64_t);
TASK_IMPL_2(MTBDD, _gate_symb_s, MTBDD, t, uint64_t, xt)
{
    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    if (mtbdd_isnode(t)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        uint32_t var = mtbdd_getvar(t);
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        if (var == xt) { 
            // Multiply the high successor by i
            return mtbdd_makenode(xt, low, my_mtbdd_symb_coef_rot2(high));
        }
        
        if (var < xt) {
            MTBDD new_high = mtbdd_false, new_low = mtbdd_false;
            if (mtbdd_isleaf(high) || (mtbdd_getvar(high) > xt)) {
                new_high = mtbdd_makenode(xt, high, my_mtbdd_symb_coef_rot2(high));
                high = new_high;
            }

            if (mtbdd_isleaf(low) || (mtbdd_getvar(low) > xt)) {
                new_low = mtbdd_makenode(xt, low, my_mtbdd_symb_coef_rot2(low));
                low = new_low;
            }

            if (new_high != mtbdd_false || new_low != mtbdd_false) {
                return mtbdd_makenode(var, low, high);
            }
        }
    }
    else { // is a leaf
        return t;
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_s(MTBDD *p_t, uint32_t xt)
{
    check_xt_root_missing(p_t, xt);
    *p_t = mtbdd_uapply(*p_t, TASK(_gate_symb_s), xt);
}


TASK_DECL_2(MTBDD, _gate_symb_t, MTBDD, uint64_t);
TASK_IMPL_2(MTBDD, _gate_symb_t, MTBDD, t, uint64_t, xt)
{
    // Partial function check
    if (t == mtbdd_false) return mtbdd_false;

    if (mtbdd_isnode(t)) {
        xt = (uint32_t)xt; // variables are uint32_t, but TASK_IMPL_2 needs 2 uint64_t
        uint32_t var = mtbdd_getvar(t);
        MTBDD high = mtbdd_gethigh(t);
        MTBDD low = mtbdd_getlow(t);

        if (var == xt) { 
            // Multiply the high successor by e^(i*pi/4)
            return mtbdd_makenode(xt, low, my_mtbdd_symb_coef_rot1(high));
        }

        if (var < xt) {
            MTBDD new_high = mtbdd_false, new_low = mtbdd_false;
            if (mtbdd_isleaf(high) || (mtbdd_getvar(high) > xt)) {
                new_high = mtbdd_makenode(xt, high, my_mtbdd_symb_coef_rot1(high));
                high = new_high;
            }

            if (mtbdd_isleaf(low) || (mtbdd_getvar(low) > xt)) {
                new_low = mtbdd_makenode(xt, low, my_mtbdd_symb_coef_rot1(low));
                low = new_low;
            }

            if (new_high != mtbdd_false || new_low != mtbdd_false) {
                return mtbdd_makenode(var, low, high);
            }
        }
    }
    else { // is a leaf
        return t;
    }

    return mtbdd_invalid; // Recurse deeper
}

void gate_symb_t(MTBDD *p_t, uint32_t xt)
{
    check_xt_root_missing(p_t, xt);
    *p_t = mtbdd_uapply(*p_t, TASK(_gate_symb_t), xt);
}


void gate_symb_h(MTBDD *p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = my_mtbdd_symb_t_xt_comp(t, xt);  // Txt_c
    mtbdd_protect(&res);

    MTBDD t_xt = my_mtbdd_symb_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    MTBDD inter_res = my_mtbdd_symb_b_xt_comp_mul(t_xt, xt); // Bxt_c * Txt
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&t_xt);

    res = my_mtbdd_symb_plus(res, inter_res); // Txt_c + (Bxt_c * Txt)

    inter_res = my_mtbdd_symb_b_xt_mul(t, xt); // Bxt * T
    mtbdd_unprotect(&t);
    res = my_mtbdd_symb_minus(res, inter_res); // (Txt_c + Bxt_c * Txt) - Bxt * T
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);

    mpz_add_ui(cs_k, cs_k, 1); // (1/√2) * (Txt_c + Bxt_c * Txt - Bxt * T)
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
    
    inter_res = my_mtbdd_plus(bracket_left, bracket_right); // (Bxc'_c * T) + (Bxc' * (Bxt_c * Txt + Bxt * Txt_c))
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);

    inter_res = my_mtbdd_symb_b_xt_mul(inter_res, xc1); // Bxc * (Bxc'_c * T + Bxc' * (Bxt_c * Txt + Bxt * Txt_c))
    res = my_mtbdd_symb_plus(res, inter_res); // (Bxc_c * T) + (Bxc * (Bxc'_c * T + Bxc' * (Bxt_c * Txt + Bxt * Txt_c)))
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_symb_cccnot(MTBDD *p_t, uint32_t xt, uint32_t xc1, uint32_t xc2, uint32_t xc3)
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

    bracket_right = my_mtbdd_symb_b_xt_mul(inter_res, xc3); // Bxc'' * (Bxt_c * Txt + Bxt * Txt_c)
    bracket_left = my_mtbdd_symb_b_xt_comp_mul(t, xc3); // Bxc''_c * T
    inter_res = my_mtbdd_plus(bracket_left, bracket_right); // (Bxc''_c * T) + (Bxc'' * (Bxt_c * Txt + Bxt * Txt_c))

    bracket_right = my_mtbdd_symb_b_xt_mul(inter_res, xc2); // Bxc' * (Bxc''_c * T + Bxc'' * (Bxt_c * Txt + Bxt * Txt_c))
    bracket_left = my_mtbdd_symb_b_xt_comp_mul(t, xc2); // Bxc'_c * T
    mtbdd_unprotect(&t);
    inter_res = my_mtbdd_plus(bracket_left, bracket_right); // (Bxc'_c * T) + (Bxc' * (Bxc''_c * T + Bxc'' * (Bxt_c * Txt + Bxt * Txt_c)))
    mtbdd_unprotect(&bracket_left);
    mtbdd_unprotect(&bracket_right);

    inter_res = my_mtbdd_symb_b_xt_mul(inter_res, xc1); // Bxc * (Bxc'_c * T + Bxc' * (Bxc''_c * T + Bxc'' * (Bxt_c * Txt + Bxt * Txt_c)))
    res = my_mtbdd_symb_plus(res, inter_res); // (Bxc_c * T) + (Bxc * (Bxc'_c * T + Bxc' * (Bxc''_c * T + Bxc'' * (Bxt_c * Txt + Bxt * Txt_c))))
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

/* end of "gates_symb.c" */
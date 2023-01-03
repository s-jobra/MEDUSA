#include "gates.h"

void gate_x(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD t_xt_comp = create_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    res = my_mtbdd_times(b_xt, t_xt_comp);  // Bxt * Txt_c 
    mtbdd_protect(&res);
    mtbdd_unprotect(&b_xt);
    mtbdd_unprotect(&t_xt_comp);

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    MTBDD t_xt = create_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    mtbdd_unprotect(&t);
    MTBDD inter_res = my_mtbdd_times(b_xt_comp, t_xt);   // Bxt_c * Txt
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&b_xt_comp);
    mtbdd_unprotect(&t_xt);
    res = my_mtbdd_plus(res, inter_res); // (Bxt * Txt_c) + (Bxt_c * Txt)
    mtbdd_unprotect(&inter_res);
    
    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_y(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD t_xt_comp = create_t_xt_comp(t, xt);
    mtbdd_protect(&t_xt_comp);
    res = my_mtbdd_times(b_xt, t_xt_comp);  // Bxt * Txt_c
    mtbdd_protect(&res);
    mtbdd_unprotect(&b_xt);
    mtbdd_unprotect(&t_xt_comp);

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    MTBDD t_xt = create_t_xt(t, xt);
    mtbdd_protect(&t_xt);
    mtbdd_unprotect(&t);
    MTBDD inter_res = my_mtbdd_times(b_xt_comp, t_xt);   // Bxt_c * Txt
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&b_xt_comp);
    mtbdd_unprotect(&t_xt);
    res = my_mtbdd_minus(res, inter_res); // (Bxt * Txt_c) - (Bxt_c * Txt)
    mtbdd_unprotect(&inter_res);

    res = my_mtbdd_coef_rot2(res); // ω² * (Bxt * Txt_c - Bxt_c * Txt) 

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_z(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    res = my_mtbdd_times(b_xt_comp, t); // Bxt_c * T
    mtbdd_protect(&res);
    mtbdd_unprotect(&b_xt_comp);

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD inter_res = my_mtbdd_times(b_xt, t); // Bxt * T
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&t);
    mtbdd_unprotect(&b_xt);
    res = my_mtbdd_minus(res, inter_res); // (Bxt_c * T) - (Bxt * T)
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
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

    res = my_mtbdd_coef_k_incr(res); // (1/√2) * (Txt_c + Bxt_c * Txt - Bxt * T)

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_s(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    res = my_mtbdd_times(b_xt_comp, t); // Bxt_c * T
    mtbdd_protect(&res);
    mtbdd_unprotect(&b_xt_comp);

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD inter_res = my_mtbdd_times(b_xt, t); // Bxt * T
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&b_xt);
    mtbdd_unprotect(&t);
    inter_res = my_mtbdd_coef_rot2(inter_res); // ω² * (Bxt * T)
    res = my_mtbdd_plus(res, inter_res); // (Bxt_c * T) + (ω² * Bxt * T)
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
}

void gate_t(MTBDD* p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    MTBDD b_xt_comp = create_b_xt_comp(xt);
    mtbdd_protect(&b_xt_comp);
    res = my_mtbdd_times(b_xt_comp, t); // Bxt_c * T
    mtbdd_protect(&res);
    mtbdd_unprotect(&b_xt_comp);

    MTBDD b_xt = create_b_xt(xt);
    mtbdd_protect(&b_xt);
    MTBDD inter_res = my_mtbdd_times(b_xt, t); // Bxt * T
    mtbdd_protect(&inter_res);
    mtbdd_unprotect(&b_xt);
    mtbdd_unprotect(&t);
    inter_res = my_mtbdd_coef_rot1(inter_res); // ω * (Bxt * T)
    res = my_mtbdd_plus(res, inter_res); // (Bxt_c * T) + (ω * Bxt * T)
    mtbdd_unprotect(&inter_res);

    *p_t = res;
    mtbdd_unprotect(&res);
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
    res = my_mtbdd_coef_k_incr(res); // (1/√2) * (T - ω² * (Bxt * Txt_c + Bxt_c * Txt))

    *p_t = res;
    mtbdd_unprotect(&res);
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

    res = my_mtbdd_coef_k_incr(res); // (1/√2) * (Txt_c + Bxt * T - Bxt_c * Txt)

    *p_t = res;
    mtbdd_unprotect(&res);
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
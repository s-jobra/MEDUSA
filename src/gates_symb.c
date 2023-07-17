#include "gates_symb.h"

void gate_symb_x(MTBDD *p_t, uint32_t xt)
{
    //mtbdd_fprintdot(stdout, *p_t); //TODO:FIXME:
    // uses the same implementation as the regular MTBDDs
    *p_t = mtbdd_uapply(*p_t, TASK(m_gate_x), xt);
}

void gate_symb_h(MTBDD *p_t, uint32_t xt)
{
    MTBDD t = *p_t;
    mtbdd_protect(&t);
    MTBDD res;

    res = my_mtbdd_symb_t_xt_comp(t, xt);  // Txt_c
    mtbdd_protect(&res);

    MTBDD t_xt = create_t_xt(t, xt);
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

/* end of "gates_symb.c" */
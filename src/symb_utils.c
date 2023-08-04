#include "symb_utils.h"

// =================
// Refine internal:
// =================

/**
 * Type for elements of the update array
 */
typedef stree_t* upd_elem_t;

/**
 * Type for elements of the refined list (old_var -> new_var mapping)
 */
typedef struct ref_elem {
    vars_t old;
    vars_t new;
    struct ref_elem *next;
} ref_elem_t;

/**
 * Type for the refined list
 */
typedef struct ref_list {
    ref_elem_t *first;
    ref_elem_t *cur;
} ref_list_t;

/**
 * Type for encapsulating all data needed during refine
 */
typedef struct rdata {
    upd_elem_t *upd;
    ref_list_t *ref;
    vmap_t *vm;
} rdata_t;

/**
 * Creates and initializes refine data
 */
static rdata_t* rdata_create(vmap_t *vm)
{
    rdata_t *rd = my_malloc(sizeof(rdata_t));

    rd->ref = my_malloc(sizeof(ref_list_t));
    rd->ref->first = NULL;
    rd->ref->cur = NULL;

    rd->upd = my_malloc(sizeof(upd_elem_t) * vm->msize);
    memset(rd->upd, 0, sizeof(upd_elem_t) * vm->msize);

    rd->vm = vm;
    return rd;
}

/**
 * Disposes refine data except for vmap (as it is used also outside refine)
 */
static void rdata_delete(rdata_t *rd)
{
    ref_elem_t *temp;
    while (rd->ref->first != NULL) {
        temp = rd->ref->first;
        rd->ref->first = temp->next;
        free(temp);
    }

    free(rd->upd); // stores only stree*, trees are freed when the value MTBDD is deleted
    free(rd);
}

/**
 * Adds new variable and its value to the update array, refine list and vmap array
 */
static void rdata_add(rdata_t *rd, vars_t old, vars_t new, stree_t *data)
{
    ref_elem_t *new_ref_elem = my_malloc(sizeof(ref_elem_t));
    new_ref_elem->old = old;
    new_ref_elem->new = new;
    new_ref_elem->next = rd->ref->first;
    rd->ref->first = new_ref_elem;
    
    //TODO: should realloc with step 1?
    rd->upd = my_realloc(rd->upd, sizeof(upd_elem_t) * (rd->vm->msize + 1));
    rd->upd[new] = data;

    vmap_add(rd->vm, old);
}

/**
 * Returns true if the two expressions are equal
 */
static inline bool expr_is_equal(stree_t *t_a, stree_t *t_b, size_t nvars)
{
    solver_data_t sdata;
    solver_data_init(&sdata, nvars);

    solver_exp_t res_a = parse_stree(t_a, &sdata);
    solver_exp_t res_b = parse_stree(t_b, &sdata);
    solver_exp_t query = solver_create_neq(&sdata, res_a, res_b);

    bool is_equal = solve(&sdata, query);

    solver_data_delete(&sdata);

    return is_equal;
}

/**
 * Returns the refined variable for the given data
 */
static vars_t refine_var_check(vars_t var, stree_t *data, rdata_t *rd)
{
    if (rd->upd[var] == NULL) {
        rd->upd[var] = data;
        return var;
    }

    if (rd->upd[var] == (void*)1) {
        // always create new var when not mtbdd_false
        if (data == (void*)1) {
            return var;
        }
    }
    else if (expr_is_equal(data, rd->upd[var], rd->vm->msize)) {
        return var;
    }

    vars_t new = rd->vm->next_var;
    rdata_add(rd, var, new, data);
    return new;
}

TASK_DECL_3(MTBDD, mtbdd_symb_refine, MTBDD*, MTBDD*, size_t);
TASK_IMPL_3(MTBDD, mtbdd_symb_refine, MTBDD*, p_map, MTBDD*, p_val, size_t, rd_raw)
{
    MTBDD map = *p_map; // ptr needed because of 'mtbdd_applyp'
    MTBDD val = *p_val;
    rdata_t *rd = (rdata_t*) rd_raw; // 'mtbdd_applyp' accepts only size_t parameter

    if (mtbdd_isleaf(map) && mtbdd_isleaf(val)) {
        sl_map_t *mdata = (sl_map_t*) mtbdd_getvalue(map);
        vars_t new_a, new_b, new_c, new_d;

        if (val == mtbdd_false) {
            new_a = refine_var_check(mdata->va, (void*)1, rd); //FIXME: is defined behaviour?? ... at least use some macro
            new_b = refine_var_check(mdata->vb, (void*)1, rd);
            new_c = refine_var_check(mdata->vc, (void*)1, rd);
            new_d = refine_var_check(mdata->vd, (void*)1, rd);
        }
        else {
            sl_val_t *vdata = (sl_val_t*) mtbdd_getvalue(val);
            new_a = refine_var_check(mdata->va, vdata->a, rd);
            new_b = refine_var_check(mdata->vb, vdata->b, rd);
            new_c = refine_var_check(mdata->vc, vdata->c, rd);
            new_d = refine_var_check(mdata->vd, vdata->d, rd);
        }

        if (new_a == mdata->va && new_b == mdata->vb && new_c == mdata->vc && new_d == mdata->vd) {
            return map;
        }

        // new symbolic var needed
        sl_map_t new_data;
        new_data.va = new_a;
        new_data.vb = new_b;
        new_data.vc = new_c;
        new_data.vd = new_d;

        MTBDD res = mtbdd_makeleaf(ltype_symb_map_id, (uint64_t) &new_data);
        return res;
    }

    return mtbdd_invalid; // Recurse deeper
}
/**
 * Computes refine on the symbolic MTBDD pair
 * 
 * @param p_map pointer to a symbolic map MTBDD
 * 
 * @param p_val pointer to a symbolic value MTBDD
 * 
 * @param rdata ptr to structure cointaining all the data needed for refine (update, refine and map data structures)
 * 
 * @param opid opid needed for the Sylvan's apply
 * 
 */
#define my_mtbdd_symb_refine(p_map, p_val, rdata, opid) \
        mtbdd_applyp(p_map, p_val, (size_t)rdata, TASK(mtbdd_symb_refine), opid)

// ========================================

void symb_init(MTBDD *circ, mtbdd_symb_t *symbc)
{
    size_t msize = 4 * (mtbdd_leafcount(*circ) + 1); // does not count F, multiplied because one var \
                                                        is needed for every coefficient
    vmap_init(&(symbc->vm), msize);

    symbc->map = my_mtbdd_to_symb_map(*circ, symbc->vm);
    mtbdd_protect(&(symbc->map));
    symbc->val = my_mtbdd_map_to_symb_val(symbc->map);
    mtbdd_protect(&(symbc->val));

    mpz_init(cs_k);
}

bool symb_refine(mtbdd_symb_t *symbc)
{
    rdata_t *rdata = rdata_create(symbc->vm);

    MTBDD refined = my_mtbdd_symb_refine(symbc->map, symbc->val, rdata, cache_next_opid());
    mtbdd_protect(&refined);
    bool is_finished = (rdata->ref->first == NULL);
    if (!is_finished) {
        symbc->map = refined;
        symbc->val = my_mtbdd_map_to_symb_val(refined);
        mtbdd_unprotect(&refined);
    }

    rdata_delete(rdata);
    return is_finished;
}

void symb_eval(MTBDD *circ,  mtbdd_symb_t *symbc, uint32_t iters)
{
    coef_t *new_map = my_malloc(sizeof(coef_t) * symbc->vm->msize);
    for (int i; i < symbc->vm->msize; i++) {
        mpz_init(new_map[i]);
    }
    coef_t *temp_map;

    //TODO: FIXME:
    // FILE *out = fopen("res.dot", "w");
    // mtbdd_fprintdot(out, symbc->val);
    // fclose(out);

    for (uint32_t i = 0; i < iters; i++) {
        my_mtbdd_update_map(symbc->map, symbc->val, symbc->vm->map, new_map);

        // swap maps
        temp_map = symbc->vm->map;
        symbc->vm->map = new_map;
        new_map = temp_map;
    }

    *circ = my_mtbdd_from_symb(symbc->map, symbc->vm->map);

    mpz_mul_ui(cs_k, cs_k, (unsigned long)iters);
    mpz_add(c_k, c_k, cs_k);

    for (int i = 0; i < symbc->vm->msize; i++) {
        mpz_clear(new_map[i]);
    }
    free(new_map);
    vmap_delete(symbc->vm);
    mpz_clear(cs_k);
}

void cs_k_reset()
{
    mpz_set_ui(cs_k, 0);
}

/* end of "symb_utils.c" */
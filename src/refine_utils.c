#include "refine_utils.h"

rdata_t* rdata_create(vmap_t *vm)
{
    rdata_t *rd = my_malloc(sizeof(rdata_t));

    rd->ref = my_malloc(sizeof(ref_list_t));
    rd->ref->first = NULL;
    rd->ref->cur = NULL;

    rd->upd = my_malloc(sizeof(upd_elem_t) * vm->msize);
    for (int i = 0; i < vm->msize; i++) {
        rd->upd[i] = NULL;
    }

    rd->vm = vm;
    return rd;
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

void rdata_delete(rdata_t *rd)
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

//FIXME: add comment
static bool expr_is_equal(stree_t *t_a, stree_t *t_b)
{
    return (t_a == t_b); //FIXME: add z3 call
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

    if (expr_is_equal(data, rd->upd[var])) {
        return var;
    }

    vars_t new = rd->vm->next_var;
    rdata_add(rd, var, new, data);
    return new;
}

TASK_IMPL_3(MTBDD, mtbdd_symb_refine, MTBDD*, p_map, MTBDD*, p_val, size_t, rd_raw)
{
    MTBDD map = *p_map; // ptr needed because of 'mtbdd_applyp'
    MTBDD val = *p_val;
    rdata_t *rd = (rdata_t*) rd_raw; // 'mtbdd_applyp' accepts only size_t parameter

    if (mtbdd_isleaf(map) && mtbdd_isleaf(val)) {
        sl_map_t *mdata = (sl_map_t*) mtbdd_getvalue(map);
        sl_val_t *vdata = (sl_val_t*) mtbdd_getvalue(val);

        vars_t new_a = refine_var_check(mdata->va, vdata->a, rd);
        vars_t new_b = refine_var_check(mdata->vb, vdata->b, rd);
        vars_t new_c = refine_var_check(mdata->vc, vdata->c, rd);
        vars_t new_d = refine_var_check(mdata->vd, vdata->d, rd);
        
        if (new_a == mdata->va && new_b == mdata->vb && new_c == mdata->vc && new_d == mdata->vd) {
            return map;
        }

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

/* end of "refine_utils.c" */
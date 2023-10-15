#include "symexp_list.h"

symexp_list_t* symexp_list_create()
{
    symexp_list_t *res = my_malloc(sizeof(symexp_list_t));
    res->active = NULL;
    res->first = NULL;
}

void symexp_list_first(symexp_list_t *l)
{
    if (l) {
        l->active =  l->first;
    }
}

void symexp_list_next(symexp_list_t *l)
{
    if (l) {
        l->active = (l->active)? l->active = l->active->next : NULL;
    }
}

void symexp_list_insert_first(symexp_list_t *l, symexp_val_t *val)
{
    if(l) {
        symexp_el_t *new_el = my_malloc(sizeof(symexp_el_t));
        new_el->data = my_malloc(sizeof(symexp_val_t));
        new_el->data->var = val->var;
        mpz_init_set(new_el->data->coef, val->coef);
        new_el->next = l->first;
        l->first = new_el;
    }
}

void symexp_list_insert_after(symexp_list_t *l, symexp_val_t *val)
{
    if(l && l->active){
        symexp_el_t *new_el = my_malloc(sizeof(symexp_el_t));
        new_el->data = my_malloc(sizeof(symexp_val_t));
        new_el->data->var = val->var;
        mpz_init_set(new_el->data->coef, val->coef);
        new_el->next = l->active->next;
        l->active->next = new_el;
    }
}

void symexp_list_neg(symexp_list_t *l)
{
    if(l){
        symexp_list_first(l);
        while (l->active) {
            mpz_neg (l->active->data->coef, l->active->data->coef);
            symexp_list_next(l);
        }
    }
}

void symexp_list_del(symexp_list_t *l)
{
    if (l) {
        symexp_el_t *tmp;
        symexp_list_first(l);
        while (l->active) {
            tmp = l->active;
            symexp_list_next(l);
            // Dealloc:
            mpz_clear(tmp->data->coef);
            free(tmp->data);
            free(tmp);
        }
        free(l);
    }
}

symexp_list_t* symexp_list_mkcpy(symexp_list_t *l)
{
    symexp_list_t *cpy = NULL;
    if (l && l->first) {    // Not empty
        cpy = my_malloc(sizeof(symexp_list_t));
        cpy->active = NULL;
        cpy->first = NULL;

        symexp_list_first(l);
        symexp_el_t *tmp;
        while (l->active != NULL) {
            tmp = my_malloc(sizeof(symexp_el_t));
            tmp->data = my_malloc(sizeof(symexp_val_t));
            tmp->data->var = l->active->data->var;
            tmp->next = NULL;
            mpz_init_set(tmp->data->coef, l->active->data->coef);

            if (cpy->first == NULL) {    // Add the first element
                cpy->first = tmp;
                symexp_list_first(cpy);
            }
            else {
                cpy->active->next = tmp;
                symexp_list_next(cpy);
            }
            symexp_list_next(l);
        }
    }
    return cpy;
}

/* end of "symexp.c" */
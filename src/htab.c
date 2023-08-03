#include "htab.h"

htab_t* htab_init(size_t n)
{
    htab_t *t = my_malloc(sizeof(htab_t));

    t->arr_ptr = my_malloc(sizeof(htab_item_t*) * n);
    memset(t->arr_ptr, 0, sizeof(htab_item_t*) * n);

    t->size = 0;
    t->arr_size = n;

    return t;
}

//TODO: change the function OR include source
/**
 * The hash function used when operating with the table items
 */
size_t htab_hash_function(const char *str) {
    uint32_t h = 0;
    const unsigned char *p;

    for(p=(const unsigned char*)str; *p!='\0'; p++) {
        h = 65599*h + *p;
    }

    return h;
}

/**
 * Changes the size of the table's array and moves items from the original lists.
 * If the new size is 0, deletes the array.
 */
void htab_resize(htab_t *t, size_t newn)
{
    if (newn == 0) {
        htab_clear(t);
        free(t->arr_ptr);
        t->arr_size = 0;
        t->size = 0;
        t->arr_ptr = NULL;
        return;
    }

    // alloc and init new array
    htab_item_t **new_arr_ptr;
    new_arr_ptr = my_malloc(newn * sizeof(htab_item_t*));
    memset(new_arr_ptr, 0, newn * sizeof(htab_item_t*));

    // index change
    htab_item_t *curr;
    htab_item_t *temp; // aux ptr to the elem in the new arr
    size_t new_index; // aux var for the new index of the current elem
    
    for (size_t i = 0; i < t->arr_size; i++) {
        curr = t->arr_ptr[i];

        while (curr != NULL) {
            new_index = htab_hash_function(curr->data.key) % newn;

            // insert into new array
            if (new_arr_ptr[new_index] == NULL) {
                new_arr_ptr[new_index] = curr;
                curr = curr->next; //preserve the original list
                new_arr_ptr[new_index]->next = NULL;
            }
            else {
                temp = new_arr_ptr[new_index];
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = curr;
                curr = curr->next; // preserve the original list
                temp->next->next = NULL;
            }
        }
    }

    free(t->arr_ptr);
    t->arr_ptr = new_arr_ptr;
    t->arr_size = newn;
    return;
}

void htab_lookup_add(htab_t *t, htab_key_t key)
{
    htab_item_t *item = t->arr_ptr[htab_hash_function(key) % t->arr_size];

    // find the item
    while (item != NULL) {
        if (strcmp(item->data.key,key) == 0) {
            item->data.value++;
            return;
        }
        item = item->next;
    }

    item = my_malloc(sizeof(htab_item_t));
    
    // item init
    char *key_temp = my_malloc(sizeof(char) * (strlen(key) + 1));
    item->data.key = strcpy(key_temp, key);
    item->data.value = 1;
    item->next = NULL;

    // insert new item into the table
    size_t new_index = htab_hash_function(key) % t->arr_size;
    if (t->arr_ptr[new_index] == NULL) {
        t->arr_ptr[new_index] = item;
    }
    else {
        htab_item_t * temp = t->arr_ptr[new_index];
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = item;
    }

    t->size++;
    if (((t->size + 0.0)/ t->arr_size) > AVG_LEN_MAX) { // +0.0 because of non-integer division
        htab_resize(t, t->size * 2);
    }
}

void htab_print_all(htab_t *t, FILE *output)
{
    fprintf(output, "Sampled results:\n");

    htab_item_t *curr;
    for (size_t i = 0; i < t->arr_size; i++) {
        curr = t->arr_ptr[i];

        while (curr != NULL) {
            fprintf(output,"    \'");
            // key stored as LSBF
            for(int i=strlen(curr->data.key); i >= 0 ;i--) {
                putc(curr->data.key[i], output);
            }
            fprintf(output,"\'    %d\n", curr->data.value);
            curr = curr->next;
        }
    }
    return;
}

void htab_clear(htab_t *t)
{
    htab_item_t *curr;

    for (size_t i = 0; i < t->arr_size; i++) {
        while (t->arr_ptr[i] != NULL) {
            curr = t->arr_ptr[i];
            t->arr_ptr[i] = t->arr_ptr[i]->next;
            
            free((void*)curr->data.key);
            free(curr);
        }
    }
}

void htab_free(htab_t *t)
{
    if (t->arr_ptr != NULL) {
        htab_clear(t);
        free(t->arr_ptr);
    }
    free(t);
}

/* end of "htab.c" */
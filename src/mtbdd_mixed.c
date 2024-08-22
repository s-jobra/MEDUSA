#include <string.h>
#include "mtbdd_mixed.h"
#include "mtbdd_symb_val.h"
#include "mtbdd_out.h"
#include "error.h"
#include "hash.h"

/// leaf type id for mixed value mtbdds
uint32_t ltype_mixed_id;
/// coefficient k common for all MTBDD leaf values for mixed value mtbdd
coefm_k_t cm_k;

/* SETUP */
void init_my_leaf_mixed()
{
    ltype_mixed_id = sylvan_mt_create_type();

    sylvan_mt_set_create(ltype_mixed_id, my_leaf_mixed_create);
    sylvan_mt_set_destroy(ltype_mixed_id, my_leaf_mixed_destroy);
    sylvan_mt_set_equals(ltype_mixed_id, my_leaf_mixed_equals);
    sylvan_mt_set_to_str(ltype_mixed_id, my_leaf_mixed_to_str);
    sylvan_mt_set_hash(ltype_mixed_id, my_leaf_mixed_hash);
}

/* CUSTOM HANDLES FOR SYLVAN */
void my_leaf_mixed_create(uint64_t *ldata_p_raw)
{
    leaf_mix_t** ldata_p = (leaf_mix_t**)ldata_p_raw; // Leaf data type is uint64_t, we store there ptr to our actual data
    
    leaf_mix_t* orig_ldata = *ldata_p;
    leaf_mix_t* new_ldata = (leaf_mix_t*)my_malloc(sizeof(leaf_mix_t));

    for (int i; i < N_COEFS; i++) {
        new_ldata->coefs[i].is_symb = orig_ldata->coefs[i].is_symb;
        if (new_ldata->coefs[i].is_symb) {
            new_ldata->coefs[i].symb = orig_ldata->coefs[i].symb;
        }
        else {
            mpz_init_set(new_ldata->coefs[i].num, orig_ldata->coefs[i].num);
            new_ldata->coefs[i].symb = NULL;
        }
    }

    *ldata_p = new_ldata;
}

void my_leaf_mixed_destroy(uint64_t ldata)
{
    leaf_mix_t *data_p = (leaf_mix_t*) ldata; // Data in leaf = pointer to my data
    
    // FIXME: check if the following is true, depends on how exactly will the htab be used during verification
    // Called only after htab_clear() -> when leaf data has already been deallocated
    free(data_p);
}

int my_leaf_mixed_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw)
{
    leaf_mix_t *ldata_a = (leaf_mix_t *) ldata_a_raw;
    leaf_mix_t *ldata_b = (leaf_mix_t *) ldata_b_raw;

    bool is_equal = true;
    
    for (int i = 0; i < N_COEFS; i++) {
        if (ldata_a->coefs[i].is_symb == true && ldata_b->coefs[i].is_symb == true) {
            is_equal = symexp_cmp(ldata_a->coefs[i].symb, ldata_b->coefs[i].symb);
        }
        else if (ldata_a->coefs[i].is_symb == false && ldata_b->coefs[i].is_symb == false) {
            is_equal = !mpz_cmp(ldata_a->coefs[i].num, ldata_b->coefs[i].num);
        }
        else {
            // Different types cannot be equal
            is_equal = false;
        }

        if (!is_equal) {
            break;
        }
    }

    return is_equal;
}

char* my_leaf_mixed_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize)
{
    (void) complemented;
    leaf_mix_t *ldata = (leaf_mix_t*) ldata_raw;

    // For each coefficient, print into the buffer according to type and join the buffers in the end
    assert(MAX_NUM_LEN < MAX_ST_TO_STR_LEN);         // So symbolic buffer sizes can be used in both cases
    char prep_buf[N_COEFS][MAX_ST_TO_STR_LEN] = {0}; // 2D array with buffer for each coefficient
    int chars_written;
    for (int i = 0; i < N_COEFS; i++) {
        printf("printing amplitude %d\n", i);
        if (ldata->coefs[i].is_symb) {
            chars_written = snprintf(prep_buf[i], sizeof(prep_buf[i]), "%s", symexp_to_str(ldata->coefs[i].symb));
        }
        else {
            // TODO: implement GCD division first?
            // Check if the number isn't too big
            if (mpz_sizeinbase(ldata->coefs[i].num, 10) > MAX_NUM_LEN) {
                chars_written = snprintf(prep_buf[i], sizeof(prep_buf[i]), VAR_NAME_FMT, lnum_map_add(&(ldata->coefs[i].num), 0));
            }
            else {
                gmp_snprintf(prep_buf[i], MAX_NUM_LEN + 2, "%Zd", ldata->coefs[i].num);
                continue; // Will always fit as long as MAX_NUM_LEN < MAX_ST_TO_STR_LEN, so no check needed
            }
        }

        if (chars_written >= (MAX_NUM_LEN + 2)) {
            error_exit("The allocated string length for a single coefficient of the mixed value leaf output has not been sufficient.\n");
        }
        else if (chars_written < 0) {
            error_exit("An encoding error has occured when producing a single coefficient output of the mixed-value leaf.\n");
        }
    }

    char ldata_string[MAX_SYMB_LEAF_STR_LEN] = {0};
    assert(N_COEFS == 4);
    chars_written = gmp_snprintf(ldata_string, MAX_SYMB_LEAF_STR_LEN, "(1/√2)^(%Zd) * ((%s) + (%s)ω + (%s)ω² + (%s)ω³)", \
                                 cm_k, prep_buf[0], prep_buf[1], prep_buf[2], prep_buf[3]);

    // Was string truncated?
    if (chars_written >= MAX_SYMB_LEAF_STR_LEN) {
        error_exit("Allocated string length for mixed value leaf output has not been sufficient.\n");
    }
    else if (chars_written < 0) {
        error_exit("An encoding error has occured when producing mixed value leaf output.\n");
    }

    // Is buffer large enough?
    if (chars_written < sylvan_bufsize) {
        memcpy(sylvan_buf, ldata_string, chars_written * sizeof(char));
        sylvan_buf[chars_written] = '\0';
        return sylvan_buf;
    }
    
    // Else return newly allocated string
    char *new_buf = (char*)my_malloc((chars_written + 1) * sizeof(char));
    memcpy(new_buf, ldata_string, chars_written * sizeof(char));
    new_buf[chars_written] = '\0';
    return new_buf;
}

uint64_t my_leaf_mixed_hash(const uint64_t ldata_raw, const uint64_t seed)
{
    leaf_mix_t *ldata = (leaf_mix_t*) ldata_raw;

    //FIXME: check if good (possible mapping problems with specific numbers/symbolic - too much overlay)
    uint64_t val = seed;
    for (int i = 0; i < N_COEFS; i++) {
        val = (ldata->coefs[i].is_symb)? MY_HASH_COMB(val, ldata->coefs[i].symb) : MY_HASH_COMB_GMP(val, ldata->coefs[i].num);
    }
    return val;
}

/* end of "mtbdd_mixed.c" */
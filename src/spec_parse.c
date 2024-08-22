#include <string.h>
#include <ctype.h>
#include "mtbdd_mixed.h"
#include "symexp.h"
#include "error.h"

/// String length for max supported length of symbolic variable identifier (includes '\0')
/// Chosen as max value that is lesser than internal variable mapping max: 52 * 62^9 < uint64_max
#define MAX_IDENTIF_LEN (10+1)
/// String length for max supported number of decimals of a number (includes '\0')
#define MAX_INPUT_NUM_LEN (MAX_NUM_LEN+1)

/// Type for specifying token types
typedef enum {
    TOKEN_NUM,          // For specific amplitudes
    TOKEN_SYMB_VAR,     // For symbolic amplitudes
    TOKEN_TUPLE_SEP,    // , (separator between 5-tuple values)
    TOKEN_AMPL_SEP,     // + (separator between amplitudes)
    TOKEN_LPAR,         // (
    TOKEN_RPAR,         // )
    TOKEN_TENSOR_PROD,  // * (tensor product operator) FIXME: add states or a single TOKEN_STAR?
    TOKEN_KET_START,    // |
    TOKEN_BASE_VEC,     // Binary string or * for the remaining states
    TOKEN_KET_END,      // >
    TOKEN_END,          // EOF
    TOKEN_INVALID
} token_type_t;
#define TUPLE_SEP ','
#define AMPL_SEP '+'
#define TENSOR_PROD '*'
#define KET_START '|'
#define KET_END '>'
#define REMAINIG_STATES '*'

/// Type for token representation
typedef struct token {
    token_type_t type;
    char *value; /// Token value - either number (dec/bin), symbolic variable or start of invalid token, null for other token types
} token_t;

/**
 * Scanner, returns next token from the given file
 */
static token_t get_token(FILE *spec)
{
    token_t tok;    //FIXME: pass by value or reference?
    tok.value = NULL;

    int c = fgetc(spec);

    while (isspace(c)) {
        c = fgetc(spec);
    }

    if (c == TUPLE_SEP) {
        tok.type = TOKEN_TUPLE_SEP;
    }
    //FIXME: add all rules
    else {
        tok.type = TOKEN_INVALID;
        // Fill the token value with 10 other character so the start of the invalid token can be printed for debugging
        tok.value = calloc(MAX_IDENTIF_LEN, sizeof(char));
        tok.value[0] = c; 
        for (int i = 1; i < MAX_IDENTIF_LEN; i++) {
            c = fgetc(spec);
            if (c == EOF) {
                break;
            }
            tok.value[i] = c;
        }
    }
    return tok;
}

/**
 * FIXME: should even be separate function?
 */
void parse_spec(FILE *spec,  MTBDD *circ)
{
    token_t tok;
    assert(MAX_IDENTIF_LEN <= MAX_NUM_LEN); // Check so MAX_NUM_LEN can be used for both cases
    char ampl[N_COEFS][MAX_NUM_LEN];
    bool is_symbolic = false;

    while ((tok = get_token(spec)).type != TOKEN_END) {
        if (tok.type == TOKEN_INVALID) {
            error_exit("Invalid token in precondition specification (starts here %s...).\n", tok.value);
        }
        //FIXME: finish this
        /*
        cases:
        - first first amplitude: also init k
        - first amplitude: construct a new MTBDD (cube)
        - other amplitudes: modify the MTBDD (union_cube), check k
        - tensor product: apply on the first of the MTBDDs
        
        rules:
        1 S -> a |00...> + a |00...> + ... a |11...>
        2 S -> a |00...> + a |00...> + ... a |*>
        3 S -> (S) * (S)

        ... analyza shora dolu: pokud token ( call 3 (zde rekurzivni call na 3/1), else call 1
        
        v 1 zaprve ziskej 1. amplitudu a vytvor  MTBDD, pro zbytek parsuj dal dokud neni END nebo ) a pripojuj ke stromu
        u kazde base kontrola delky + jestli nebyla jeste, pokud je konec a nebyla *, zkontroluj jestli byly vsechny baze (podle vektoru pro assignement *)

        ve 3 ziskej obe mtbdd a slep je dohromady

        takze asi funkce start, pote state_definition a tensor_product, ve state definition asi podfunkce create_mtbdd, add_amplitude


        1. get token, should be ( or num/identif
        2. now should be sep, num/identif, sep, ... 5 times
        3. mtbdd construction,
        */
    }
}

// ========================================

void init_sylvan_verif()
{
    init_my_leaf_mixed();
}

void init_from_spec(FILE *spec,  MTBDD *circ)
{
    //FIXME: temp, testing MTBDD init functionality only
    symexp_htab_init(1LL<<17);
    mpz_init(cm_k);
    BDDSET variables = mtbdd_set_empty();
    for (uint32_t i = 0; i < 4; i++) {
        variables = mtbdd_set_add(variables, i);
    }
    leaf_mix_t ldata0;
    for (int i = 0; i < N_COEFS; i++) {
        if (i == 1) {
            mpz_init_set_ui(ldata0.coefs[i].num, 1);
            ldata0.coefs[i].is_symb = false;
        }
        else {
            ldata0.coefs[i].symb = symexp_init(i);
            ldata0.coefs[i].is_symb = true;
        }
    }
    leaf_mix_t ldata1;
    for (int i = 0; i < N_COEFS; i++) {
        if (i == 2 || i == 3) {
            mpz_init_set_ui(ldata1.coefs[i].num, 1);
            ldata1.coefs[i].is_symb = false;
        }
        else {
            ldata1.coefs[i].symb = symexp_init(i+4);
            ldata1.coefs[i].is_symb = true;
        }
    }
    MTBDD leaf0  = mtbdd_makeleaf(ltype_mixed_id, (uint64_t) &ldata0);
    MTBDD leaf1  = mtbdd_makeleaf(ltype_mixed_id, (uint64_t) &ldata1);

    uint8_t leaf_symbol0[4];
    memset(leaf_symbol0, 0, 4*sizeof(uint8_t));
    uint8_t leaf_symbol1[4];
    for (uint32_t i = 0; i < 4; i++) {
        leaf_symbol1[i] = 1;
    }

    *circ = mtbdd_cube(variables, leaf_symbol0, leaf0);
    *circ = mtbdd_union_cube(*circ, variables, leaf_symbol1, leaf1);
    //FIXME: remove above
    // 1. create cube from the first encountered base vector and its amplitude
    // 2. for other amplitudes at this level, traverse the MTBDD and add a leaf - maybe can use mtbdd_union_cube?
    //    if it's *, change basically all mtbdd_false except for the specified ones? if union_cube can be used, use the previously used cubes (hold some array that is like not assigned yet - initially all 2, and alter it as we encounter bases)
    //    ideally start the cube from *, but idk how - A)specification has to start with *, B) we start from the end - some stack? ... can use call stack here?
    // 3. tensor product - (union cube and/)or custom traverse procedure that slaps them under each other
}

/* end of "spec_parse.c" */
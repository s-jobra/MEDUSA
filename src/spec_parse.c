#include <string.h>
#include <ctype.h>
#include "mtbdd_mixed.h"
#include "symexp.h"
#include "error.h"

/// Max supported length of symbolic variable identifier
/// Chosen as max value that is lesser than internal variable mapping max: 52 * 62^9 < uint64_max
#define MAX_IDENTIF_LEN 10
/// Max supported number of digits in an integer/binary number
#define MAX_BIN_NUM_LEN 500

/// Type for specifying token types
typedef enum {
    TOKEN_COMMA,        // , (separator between tuple values)
    TOKEN_PLUS,         // + (separator between amplitudes)
    TOKEN_LPAR,         // (
    TOKEN_RPAR,         // )
    TOKEN_TENSOR_PROD,  // x (tensor product operator)
    TOKEN_KET_START,    // |
    TOKEN_KET_END,      // >
    TOKEN_STAR,         // *
    TOKEN_K_DEF,        // "k=" (accepts both with and without ws between chars)
    TOKEN_Q_ORDER,      // "Qubit order:"
    TOKEN_VAR_CONSTR,   // "Variable constraints:" + block of SMT constraints
    TOKEN_SYM_VAR,      // Symbolic variable identifiers
    TOKEN_INT,          // Integers (signed)
    TOKEN_UINT,          // Unsigned integers (for qubit order)
    TOKEN_BIN_STR,      // Base vector (binary string)
    TOKEN_NEWLINE,      // \n
    TOKEN_END,          // EOF
    TOKEN_INVALID       // unrecognized token
} token_type_t;

/// Type for token representation
typedef struct token {
    token_type_t type;
    char *value; /// Token value - string with number (dec/bin), symbolic variable or start of invalid token, null for other token types
} token_t;

/**
 * Fills the given token for the case of TOKEN_SYM_VAR, includes the string read so far
 * (separate function because some TOKEN_SYM_VAR can be treated as another token before recognizing).
 */
static void fill_token_symb_var(token_t *tok, FILE *in, char *str_read)
{
    tok->value = my_calloc(MAX_IDENTIF_LEN + 1, sizeof(char));
    if (strlen(str_read) > MAX_IDENTIF_LEN) {
        error_exit("Max symbolic variable identifier length (%d characters) exceeded in specification.\n", MAX_IDENTIF_LEN);
    }
    strcpy(tok->value, str_read);

    int c;
    int i = strlen(str_read);
    while(isalnum(c = fgetc(in))) {
        if (i == MAX_IDENTIF_LEN) {
            error_exit("Max symbolic variable identifier length (%d characters) exceeded in specification.\n", MAX_IDENTIF_LEN);
        }
        tok->value[i] = c;
        i++;
    }
    if (c == EOF) {
        putc(EOF, in); // So we can return TOKEN_END next time
    }
    else if (!isspace(c)) {
        tok->type = TOKEN_INVALID;
        if (i < MAX_IDENTIF_LEN) {
            tok->value[i] = c; // Append for debugging if possible
        }
        return;
    }

    tok->type = TOKEN_SYM_VAR;
}

/**
 * Scanner, fills the given token with the next token from the stream
 */
static void get_next_token(token_t *tok, FILE *in)
{
    tok->value = NULL;
    int c;

    do {
        c = fgetc(in);
    } while (isspace(c));

    // Unambiguous single char tokens
    if (c == EOF) {
        tok->type = TOKEN_END;
    }
    else if (c == '\n') {
        tok->type = TOKEN_NEWLINE;
    }
    else if (c == ',') {
        tok->type = TOKEN_COMMA;
    }
    else if (c == '+') {
        tok->type = TOKEN_PLUS;
    }
    else if (c == '(') {
        tok->type = TOKEN_LPAR;
    }
    else if (c == ')') {
        tok->type = TOKEN_RPAR;
    }
    else if (c == '|') {
        tok->type = TOKEN_KET_START;
    }
    else if (c == '>') {
        tok->type = TOKEN_KET_END;
    }
    else if (c == '*') {
        tok->type = TOKEN_STAR;
    }
    // Numbers (int, uint or binary string)
    else if (isdigit(c) || c == '-') {
        bool is_unsigned = (c == '-')? false : true;
        bool is_binary = is_unsigned; // Negative integers are never binary bases, in other cases sets initially true
        assert(MAX_BIN_NUM_LEN >= MAX_NUM_LEN); // So we can use the same array size for both cases
        tok->value = my_calloc(MAX_BIN_NUM_LEN+1, sizeof(char));
        int i = 0;
        do {
            is_binary = is_binary && ((c == '0') || (c == '1'));
            if (is_binary && i == MAX_BIN_NUM_LEN) {
                error_exit("Specification exceeds max supported number of digits for binary numbers (%d digits).\n", MAX_BIN_NUM_LEN);
            }
            else if (!is_binary && i >= MAX_NUM_LEN) { // >= because maybe we treated as a binary number before
                error_exit("Specification exceeds max supported number of digits for integers (%d digits).\n", MAX_NUM_LEN);
            }
            tok->value[i] = c;
            i++;
        } while (isdigit(c = fgetc(in)));
        if (c == EOF) {
            putc(EOF, in); // So we can return TOKEN_END next time
        }
        // Invalid token?
        if (!isspace(c) && c != EOF) {
            tok->type = TOKEN_INVALID;
            tok->value[i] = c; // Append for debugging
        }
        else if (is_binary) {
            tok->type = TOKEN_BIN_STR;
        }
        else if (is_unsigned) {
            tok->type = TOKEN_UINT;
        }
        else {
            tok->type = TOKEN_INT;
        }
    }
    // Either single char token or identifier
    else if (c == 'x') {
        c = fgetc(in);
        if (c == EOF) {
            putc(EOF, in); // So we can return TOKEN_END next time
        }
        else if (!isspace(c)) {
            // Is actually symbolic variable identifier
            char str_read[3] = {0};
            str_read[0] = 'x';
            str_read[1] = c;
            fill_token_symb_var(tok, in, str_read);
            return;
        }
        tok->type = TOKEN_TENSOR_PROD;
    }
    else if (c == 'k') {
        bool ws_skipped = false;
        c = fgetc(in);
        while (isspace(c)) {
            c = fgetc(in);
            ws_skipped = true;
        }
        if (c == '=') {
            tok->type = TOKEN_K_DEF;
        }
        else if (!ws_skipped){ // Try if not identifier
            char str_read[3] = {0};
            str_read[0] = 'x';
            str_read[1] = c;
            fill_token_symb_var(tok, in, str_read);
        }
        else {
            if (ws_skipped) {
                putc(' ', in);
            }
            c = 'k';
            goto token_invalid;
        }
    }
    else if (c == 'Q') {
        //FIXME: finish
        tok->type = TOKEN_Q_ORDER;
    }
    else if (c == 'V') {
        //FIXME: finish
        tok->type = TOKEN_VAR_CONSTR;
    }
    // Symbolic variable identifier
    else if (isalpha(c)) {
        char str_read[2] = {0};
        str_read[0] = c;
        fill_token_symb_var(tok, in, str_read);
    }
    else {
    token_invalid:
        tok->type = TOKEN_INVALID;
        // Fill the token value with 10 other character so the start of the invalid token can be printed for debugging
        tok->value = my_calloc((MAX_IDENTIF_LEN + 1), sizeof(char));
        tok->value[0] = c; 
        for (int i = 0; i < MAX_IDENTIF_LEN; i++) {
            c = fgetc(in);
            if (c == EOF) {
                break;
            }
            tok->value[i] = c;
        }
    }
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

    while (true) {
        get_next_token(&tok, spec);
        if (tok.type == TOKEN_END) {
            break;
        }
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
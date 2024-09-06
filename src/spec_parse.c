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
/// Variable constraints definition keyword
#define SYM_VAR_KEYWORD "Constraints:"
/// Length of variable constraints definition keyword
#define SYM_VAR_KEYWORD_LEN strlen(SYM_VAR_KEYWORD)
/// Max supported number of characters in variable constraint smt block
#define MAX_SMT_BLOCK_LEN 1000

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
    TOKEN_VAR_CONSTR,   // "Constraints:" + block of SMT constraints
    TOKEN_SYM_VAR,      // Symbolic variable identifiers
    TOKEN_INT,          // Integers (signed)
    TOKEN_UINT,          // Unsigned integers (for qubit order)
    TOKEN_BIN_STR,      // Base vector (binary string)
    TOKEN_END,          // EOF
    TOKEN_INVALID       // Unrecognized token
} token_type_t;

/// Separator between tuple values/qubit order indices (for checking if the current token is a valid symbolic variable)
#define TUPLE_SEP ','
/// Ket start character (needed for TOKEN_SYM_VAR look ahead check)
#define KET_START '|'

/// Type for token representation
typedef struct token {
    token_type_t type;
    char *value; /// Token value - string with number (dec/bin), symbolic variable or start of invalid token, null for other token types
} token_t;

/**
 * Sets the given token type as TOKEN_INVALID and sets its value to the given string.
 * If the string is NULL, it is assumed the value of the token is properly set already and only the type is changed.
 */
static void fill_token_invalid(token_t *tok, char *str)
{
    tok->type = TOKEN_INVALID;
    if (str != NULL) {
        tok->value = my_calloc(strlen(str) + 1, sizeof(char));
        strcpy(tok->value, str);
    }
}

/**
 * Fills the given token for the case of TOKEN_SYM_VAR, includes the string read so far
 * (separate function because some TOKEN_SYM_VAR can be treated as another token before recognizing).
 * Also sets the last read character if necessary.
 * @param tok token to fill in case we are not dealing with a keyword
 * @param in specification input
 * @param str_read start of the symbolic variable identifier we have already processed before entering this function (assumed to be a correct identifier so far)
 * @param read_previous flag that needs to be set to 'true' if the lexer should use the previously read character during the next invocation
 * @param last_c_read last character read during the current invocation of the lexer
 */
static void fill_tok_symb_var_from_str(token_t *tok, FILE *in, char *str_read, bool *read_previous, int *last_c_read)
{
    tok->value = my_calloc(MAX_IDENTIF_LEN + 1, sizeof(char));
    if (strlen(str_read) > MAX_IDENTIF_LEN) {
        error_exit("Max symbolic variable identifier length (%d characters) exceeded in specification.\n", MAX_IDENTIF_LEN);
    }
    strcpy(tok->value, str_read); // Always copy, always at least 1 character

    int c;
    int i = strlen(str_read);
    while(isalnum(c = fgetc(in))) {
        if (i == MAX_IDENTIF_LEN) {
            error_exit("Max symbolic variable identifier length (%d characters) exceeded in specification.\n", MAX_IDENTIF_LEN);
        }
        tok->value[i] = c;
        i++;
    }

    if (!isspace(c) && c != EOF && c != TUPLE_SEP && c != KET_START) {
        if (i < MAX_IDENTIF_LEN) {
            tok->value[i] = c; // Append for debugging if possible
        }
        fill_token_invalid(tok, NULL);
        return;
    }
    tok->type = TOKEN_SYM_VAR;
    *last_c_read = c;
    *read_previous = true;
}

/**
 * Checks if the current token is the given short keyword by checking the character that should follow after whitespace. If yes, returns true, else 
 * reads the symbolic variable or the invalid token and fills the given data structures.
 * @param start first character of the keyword (currently supports only format char-ws-c_follow)
 * @param c_follow character after whitespace that is present only in the keyword
 * @param tok token to fill in case we are not dealing with a keyword
 * @param in specification input
 * @param read_previous flag that needs to be set to 'true' if the lexer should use the previously read character during the next invocation
 * @param last_c_read last character read during the current invocation of the lexer
 */
static bool check_if_short_keyword_else_fill_token(char start, char c_follow, token_t *tok, FILE *in, bool *read_previous, int *last_c_read)
{
    int c;
    bool ws_skipped = false;
    while (isspace(c = fgetc(in))) { ws_skipped = true;}
    if (c == c_follow) {
        return true;
    }
    else if (!ws_skipped) {
        // Already read a next character from the identifier
        if (isalnum(c)) {
            char buf[3] = {0};
            buf[0] = start;
            buf[1] = c;
            fill_tok_symb_var_from_str(tok, in, buf, read_previous, last_c_read);
        }
        else {
            // Invalid
            char buf[MAX_IDENTIF_LEN + 1] = {0};
            buf[0] = start;
            for (int i = 0; i < MAX_IDENTIF_LEN; i++) {
                if (c == EOF) {
                    break;
                }
                buf[i] = c;
                c = fgetc(in);
            }
            fill_token_invalid(tok, buf);
        }
    }
    else {
        // Only identifier 'start' - set manually
        tok->type = TOKEN_SYM_VAR;
        tok->value = my_calloc(MAX_IDENTIF_LEN + 1, sizeof(char));
        tok->value[0] = start;
        *read_previous = true;
        *last_c_read = c;
    }

    return false;
}

/**
 * Checks if is case of long keyword with multiple words or if it is actually a symbolic variable. In both cases fills the given token.
 * @param keyword string containing the keyword
 * @param ttype token type corresponding to the keyword
 * @param tok token to fill
 * @param in specification input stream
 * @param read_previous flag that needs to be set to 'true' if the lexer should use the previously read character during the next invocation
 * @param last_c_read last character read during the current invocation of the lexer
 */
static void check_if_long_keyword_w_space_or_identif(char *keyword, token_type_t ttype, token_t *tok, FILE *in, bool *read_previous, int *last_c_read)
{
    int c;
    bool keyword_match = true;
    char *first_space = strchr(keyword, ' ');
    int first_space_index = first_space - keyword;
    int i = 1;

    for (; i <= first_space_index; i++) { // Starts from 1 because we know the first character is matching
        c = fgetc(in);
        if (c != keyword[i]) {
            keyword_match = false;
            break;
        }
    }

    if (keyword_match) {
        // Try to match the rest of the keyword, still possibly a symbolic variable identifier
        for (; i < strlen(keyword); i++) {
            c = fgetc(in);
            if (c != keyword[i]) {
                keyword_match = false;
                break;
            }
        }

        if (keyword_match) {
            tok->type = ttype;
            // No need to set last character read since we read only the keyword
            return;
        }
    }

    // Else either a symbolic variable or an invalid token
    char buf[i+2];
    strcpy(buf, keyword);
    buf[i] = c;
    buf[i+1] = '\0';
    bool is_alnum = true;
    for (int i = 1; i < i+2; i++) { // First character isalpha() = true
        if (!isalnum(buf[i])) {
            is_alnum = false;
            break;
        }
    }
    if (is_alnum) {
        // Symbolic variable
        fill_tok_symb_var_from_str(tok, in, buf, read_previous, last_c_read);
    }
    else {
        //Invalid
        fill_token_invalid(tok, buf); //FIXME: print longer? like full 10 chars
    }
}

/**
 * Handles case when the current token can be either a keyword or a symbolic variable
 * (parts of the keyword or the whole keyword is a possible symbolic variable identifier)
 * @param tok token to be filled
 * @param in specification input stream
 * @param start first character read by the lexer (because we read a character, we entered this handle)
 * @param read_previous flag that needs to be set to 'true' if the lexer should use the previously read character during the next invocation
 * @param last_c_read last character read during the current invocation of the lexer
 */
static void handle_alpha(token_t *tok, FILE *in, char start, bool *read_previous, int *last_c_read)
{
    if (start == 'x') {
        // Possibly tensor product
        if (check_if_short_keyword_else_fill_token('x', '(', tok, in, read_previous, last_c_read)) {
            tok->type = TOKEN_TENSOR_PROD;
            *last_c_read = '(';
            *read_previous = true;
        }
    }
    else if (start == 'k') {
        // Possibly k definition
        if (check_if_short_keyword_else_fill_token('k', '=', tok, in, read_previous, last_c_read)) {
            tok->type = TOKEN_K_DEF;
            // Not setting last read here since = is actually part of the keyword
        }
    }
    else if (start == 'Q') {
        // Possibly qubit order definition
        check_if_long_keyword_w_space_or_identif("Qubit order:", TOKEN_Q_ORDER, tok, in, read_previous, last_c_read); //FIXME: check last c read if correct + identif creating
    }
    else {
        // Loading symbolic var from the start
        tok->value = my_calloc(MAX_IDENTIF_LEN + 1, sizeof(char));
        tok->value[0] = start;

        int c;
        int i = 1;
        while(isalnum(c = fgetc(in))) {
            if (i == MAX_IDENTIF_LEN) {
                error_exit("Max symbolic variable identifier length (%d characters) exceeded in specification.\n", MAX_IDENTIF_LEN);
            }
            tok->value[i] = c;
            i++;
        }

        if (!isspace(c) && c != EOF && c != TUPLE_SEP && c != KET_START) {
            if (i < MAX_IDENTIF_LEN) {
                tok->value[i] = c; // Append for debugging if possible
            }
            fill_token_invalid(tok, NULL);
            return;
        }
        tok->type = TOKEN_SYM_VAR;
        *last_c_read = c;
        *read_previous = true;
    }
}

/**
 * Scanner, fills the given token with the next token from the stream
 */
static void get_next_token(token_t *tok, FILE *in)
{
    tok->value = NULL;
    static bool use_last_read = false;
    static int c; // Last read character

    if (!use_last_read || isspace(c)) {
        do {
            c = fgetc(in);
        } while (isspace(c));
    }
    use_last_read = false; // Do not propagate further

    // Unambiguous single char tokens
    if (c == EOF) {
        tok->type = TOKEN_END;
    }
    else if (c == TUPLE_SEP) {
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
    else if (c == KET_START) {
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
        tok->value = my_calloc(MAX_BIN_NUM_LEN + 1, sizeof(char));
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
        use_last_read = true;
        // Invalid token?
        if (!isspace(c) && c != EOF) {
            tok->value[i] = c; // Append for debugging
            fill_token_invalid(tok, NULL);
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
    // If not keyword, possibly a symbolic variable
    else if (c == 'C') {
        //FIXME: probably should move into handle_alpha() and also maybe not totaly correct (identifier too long gives different error)
        // Because we assume this, a string too long to be the keyword can never actually be an identifier
        // (important for the following conditions checking TOKEN_INVALID)
        assert(SYM_VAR_KEYWORD_LEN >= MAX_IDENTIF_LEN);

        // Check for "Constraints:" keyword
        int i = 0;
        char buf[SYM_VAR_KEYWORD_LEN + 1];
        for (int i=0; i < SYM_VAR_KEYWORD_LEN + 1; i++) {
            buf[i] = 0;
        }
        do {
            if (c == EOF || i == SYM_VAR_KEYWORD_LEN) {
                fill_token_invalid(tok, buf);
                return;
            }
            buf[i] = c;
            i++;
        } while (!isspace(c = fgetc(in)));

        // Is a constraint definition keyword?
        if (strcmp(buf, SYM_VAR_KEYWORD) == 0) {
            tok->type = TOKEN_VAR_CONSTR;
            tok->value = calloc(MAX_SMT_BLOCK_LEN + 1, sizeof(char));

            // Load the SMT block
            int i = 0;
            while ((c = fgetc(in)) != EOF) { // c before entering the loop is ws, so no need to save
                if (i == MAX_SMT_BLOCK_LEN) {
                    error_exit("Max supported number of characters in SMT constraints exceeded (%d characters).\n", MAX_SMT_BLOCK_LEN);
                }
                tok->value[i] = c;
                i++;
            }
        }
        else {
            // Check if can be identifier
            bool identif_possible = true;
            // I know first character is 'C' so no need to check
            for (int j=1; j < i; j++) {
                if (!isalnum(buf[j]) || j > MAX_IDENTIF_LEN) {
                    identif_possible = false;
                    break;
                }
            }
            if (identif_possible) {
                fill_tok_symb_var_from_str(tok, in, buf, &use_last_read, &c); //FIXME: check if is the case when only alphanum are present
            }
            else {
                fill_token_invalid(tok, buf);
            }
        }
    }
    // Can be both (start of) a keyword or a symbolic variable
    else if (isalpha(c)) {
        char str_read[2] = {0};
        str_read[0] = c;
        fill_tok_symb_var_from_str(tok, in, str_read, &use_last_read, &c); //FIXME: check if is the case when only alphanum are present
    }
    else {
        // Fill the token value with a few other characters so the start of the invalid token can be printed for debugging
        char buf[MAX_IDENTIF_LEN + 1] = {0};
        for (int i = 0; i < MAX_IDENTIF_LEN; i++) {
            if (c == EOF) {
                break;
            }
            buf[i] = c;
            c = fgetc(in);
        }
        fill_token_invalid(tok, buf);
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
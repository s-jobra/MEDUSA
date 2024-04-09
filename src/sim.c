#include "sim.h"

#define NO_ALT_END -2
#define EPSILON 0.001

/**
 * Function for number parsing from the input file (reads the number from the input until the end character is encountered)
 * Checks for two possible end characters. If only one character should be checked agains, set alt_end to NO_ALT_END.
 */
static long long parse_num(FILE *in, char end, char alt_end)
{
    int c = fgetc(in);
    char num[NUM_MAX_LEN] = {0};
    long long n;

    // Skip leading whitespace
    while (isspace(c)) {
        c = fgetc(in);
    }

    // Load number to string
    while ( c != end && c != alt_end) {
        if (c == EOF) {
            error_exit("Invalid format - reached an unexpected end of file when converting a number.\n");
        }
        else if (!isdigit(c) && c != '-') {
            // Check if isn't just trailing whitespace
            while (isspace(c)) {
                c = fgetc(in);
            }
            if (c == end || c == alt_end) {
                break;
            }
            else {
                error_exit("Invalid format - not a valid number (a non-digit character '%c' encountered while parsing a number).\n", c);
            }
        }
        else if (strlen(num) + 1 < NUM_MAX_LEN) {
            int *temp = &c;
            strncat(num, (char*)temp, 1);
        }
        else {
            error_exit("Invalid format - not a valid number (too many digits).\n");
        }
        c = fgetc(in);
    }

    // Convert to integer value
    char *ptr;
    errno = 0;
    n = strtoll(num, &ptr, 10);
    if (num == ptr || errno != 0) {
        error_exit("Invalid format - not a valid number.\n");
    }
    return n;
}

/** 
 * Function for getting the next qubit index for the command on the given line.
 */
static uint32_t get_q_num(FILE *in)
{
    int c;
    long long n;

    while ((c = fgetc(in)) != '[') {
        if (c == EOF) {
            error_exit("Invalid format - reached an unexpected end of file (expected a qubit index).\n");
        }
    }

    n = parse_num(in, ']', NO_ALT_END);
    if (n > UINT32_MAX || n < 0) {
        error_exit("Invalid format - not a valid qubit identifier.\n");
    }

    return ((uint32_t)n);
}

/**
 * Returns the number of iterations, should be called when a for loop is encountered
 */
static uint64_t get_iters(FILE *in)
{
    int c;
    long long start, end;
    long long step = 1;
    fpos_t second_num_pos;
    uint64_t iters;

    while ((c = fgetc(in)) != '[') {
        if (c == EOF) {
            error_exit("Invalid format - reached an unexpected end of file (expected a number of loop iterations).\n");
        }
    }

    start = parse_num(in, ':', NO_ALT_END);
    end = parse_num(in, ']', ':');
    if (fseek(in, -1, SEEK_CUR) != 0) {
        error_exit("Error has occured when parsing loop parameters (fseek error).\n");
    }
    if ((c = fgetc(in)) == ':') {
        step = end;
        end = parse_num(in, ']', NO_ALT_END);
    }
    
    // Note: expects 64bit long long
    //TODO: better error detection?
    if (step == 0) {
        error_exit("Invalid number of loop iterations - step must be non-zero.\n");
    }
    else if ((end == LLONG_MAX && start == LLONG_MIN) || (end == LLONG_MIN && start == LLONG_MAX)) {
        error_exit("Invalid number of loop iterations - overflow detected.\n");
    }
    else if ((end + 1 - start) % step != 0) {
        error_exit("Invalid number of loop iterations - not an integer.\n");
    }
    else if ((end < start && step > 0) || (end > start && step < 0)) {
        error_exit("Invalid number of loop iterations.\n");
    }

    iters = (end + 1 - start) / step;
    
    return ((uint64_t) iters);
}

bool sim_file(FILE *in, MTBDD *circ, int *n_qubits, int **bits_to_measure, bool *is_measure, bool opt_symb)
{
    //TODO: refactoring
    //TODO: add line counter and display in errors
    int c;
    char cmd[CMD_MAX_LEN];
    bool init = false;

    bool is_loop = false;
    fpos_t loop_start;
    mtbdd_symb_t symbc;
    uint64_t iters;

    while ((c = fgetc(in)) != EOF) {
        for (int i=0; i < CMD_MAX_LEN; i++) {
            cmd[i] = '\0';
        }

        while (isspace(c)) {
            c = fgetc(in);
        }

        if (c == EOF) {
            return init;
        }

        // Skip one-line comments
        if (c == '/') {
            if ((c = fgetc(in)) == '/') {
                while ((c = fgetc(in)) != '\n') {
                    if (c == EOF) {
                        return init;
                    }
                }
                continue;
            }
            else {
                error_exit("Invalid command, expected a one-line comment.\n");
            }
        }

        // Load the command
        do {
            if (c == EOF) {
                error_exit("Invalid format - reached an unexpected end of file when loading a command.\n");
            }
            else if (strlen(cmd) + 1 < CMD_MAX_LEN) {
                int *temp = &c;
                strncat(cmd, (char *)temp, 1);
            }
            else {
                error_exit("Invalid command (command too long).\n");
            }
        } while (!isspace(c = fgetc(in)));

        if (c == EOF) {
            error_exit("Invalid format - reached an unexpected end of file immediately after a command.\n");
        }

        // Identify the command
        if (strcmp(cmd, "OPENQASM") == 0) {}
        else if (strcmp(cmd, "include") == 0) {}
        else if (strcmp(cmd, "creg") == 0) {} //TODO: check if is valid?
        else if (strcmp(cmd, "qreg") == 0) {
            uint32_t n = get_q_num(in);
            *n_qubits = (int)n;
            *bits_to_measure = my_malloc(n * sizeof(int));
            for (int i=0; i < n; i++) {
                (*bits_to_measure)[i] = -1;
            }
            circuit_init(circ, n);
            mtbdd_protect(circ);
            init = true;
        }
        else if (init) {
            if (strcmp(cmd, "for") == 0) {
                iters = get_iters(in);
                if (iters == 0) {
                    // skip symbolic
                    while ((c = fgetc(in)) != '}') { //TODO: check for comments - shouldn't count commented }
                        if (c == EOF) {
                            error_exit("Invalid format - reached an unexpected end of file (there is an unfinished loop).\n");
                        }
                    }
                    continue;
                }
                while ((c = fgetc(in)) != '{') {
                    if (c == EOF) {
                        error_exit("Invalid format - reached an unexpected end of file at the start of a loop.\n");
                    }
                }
                is_loop = true; // TODO: allow nested loops?
                if (opt_symb) {
                    symb_init(circ, &symbc);
                }
                if (fgetpos(in, &loop_start) != 0) {
                    error_exit("Could not get the current position of the stream to mark the start of a loop.\n");
                }
                continue; // ';' not expected
            }
            else if (strcmp(cmd, "}") == 0) {
                if (!is_loop) {
                    error_exit("Invalid loop syntax - reached an unexpected end of a loop.\n");
                }
                if (!opt_symb) {
                    iters--;
                    if (!iters) {
                        is_loop = false;
                    }
                    else { // next iteration
                        if (fsetpos(in, &loop_start) != 0) {
                            error_exit("Could not set a new position of the stream to start the next iteration.\n");
                        }
                    }
                }
                else {
                    if (symb_refine(&symbc)) {
                        // is final result
                        is_loop = false;
                        symb_eval(circ, &symbc, iters); // TODO: should request gc call?
                    }
                    else {
                        if (fsetpos(in, &loop_start) != 0) {
                            error_exit("Could not set a new position of the stream during symbolic simulation.\n");
                        }
                    }
                }
                continue; // ';' not expected
            }
            else if (strcmp(cmd, "measure") == 0) {
                uint32_t qt = get_q_num(in);
                uint32_t ct = get_q_num(in);
                *is_measure = true;
                (*bits_to_measure)[qt] = ct;
            }
            else if (strcasecmp(cmd, "x") == 0) {
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_x(&symbc.val, qt) : gate_x(circ, qt);
            }
            else if (strcasecmp(cmd, "y") == 0) {
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_y(&symbc.val, qt) : gate_y(circ, qt);
            }
            else if (strcasecmp(cmd, "z") == 0) {
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_z(&symbc.val, qt) : gate_z(circ, qt);
            }
            else if (strcasecmp(cmd, "h") == 0) {
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_h(&symbc.val, qt) : gate_h(circ, qt);
            }
            else if (strcasecmp(cmd, "s") == 0) {
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_s(&symbc.val, qt) : gate_s(circ, qt);
            }
            else if (strcasecmp(cmd, "t") == 0) {
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_t(&symbc.val, qt) : gate_t(circ, qt);
            }
            else if (strcasecmp(cmd, "rx(pi/2)") == 0) {
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_rx_pihalf(circ, qt) : gate_rx_pihalf(circ, qt);
            }
            else if (strcasecmp(cmd, "ry(pi/2)") == 0) {
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_ry_pihalf(circ, qt) : gate_ry_pihalf(circ, qt);
            }
            else if (strcasecmp(cmd, "cx") == 0) {
                uint32_t qc = get_q_num(in);
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_cnot(&symbc.val, qt, qc) : gate_cnot(circ, qt, qc);
            }
            else if (strcasecmp(cmd, "cz") == 0) {
                uint32_t qc = get_q_num(in);
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_cz(&symbc.val, qt, qc) : gate_cz(circ, qt, qc);
            }
            else if (strcasecmp(cmd, "ccx") == 0) {
                uint32_t qc1 = get_q_num(in);
                uint32_t qc2 = get_q_num(in);
                uint32_t qt = get_q_num(in);
                (opt_symb && is_loop)? gate_symb_toffoli(&symbc.val, qt, qc1, qc2) : gate_toffoli(circ, qt, qc1, qc2);
            }
            else if (strcasecmp(cmd, "mcx") == 0) { // supports 2 and 3 control qubits
                qparam_list_t* qparams = qparam_list_create();
                
                // Read all control qubits and the target qubit and save it in qparams
                while(true) {
                    qparam_list_insert_first(qparams, get_q_num(in));
                    c = fgetc(in);
                    while (isspace(c)) {
                        c = fgetc(in);
                    }

                    if (c == ',') {
                        continue; // additional qubit indices are present in the file
                    }
                    else if (c == ';') {
                        break; // all qubit parameters loaded
                    }
                    else {
                        error_exit("Invalid 'mcx' gate syntax.\n");
                    }
                }

                (opt_symb && is_loop)? gate_symb_mcx(&symbc.val, qparams) : gate_mcx(circ, qparams);
                
                qparam_list_del(qparams);
                continue; // ';' already encountered
            }
            else if (strcasecmp(cmd, "cswap") == 0) {
                uint32_t qc = get_q_num(in);
                uint32_t qt1 = get_q_num(in);
                uint32_t qt2 = get_q_num(in);
                (opt_symb && is_loop)? error_exit("Gate 'cswap' does not support symbolic simulation.\n") : gate_fredkin(circ, qt1, qt2, qc); //TODO:
            }
            else {
                error_exit("Invalid command '%s'.\n", cmd);
            }
        }
        else {
            error_exit("Circuit not initialized.\n");
        }

        // Skip all remaining characters on the currently read line
        while ((c = fgetc(in)) != ';') {
            if (c == EOF) {
                error_exit("Invalid format - reached an unexpected end of file (expected ';' to end the current line).\n");
            }
        }
    } // while

    return init;
}

void measure_all(unsigned long samples, FILE *output, MTBDD circ, int n, int *bits_to_measure)
{
    prob_t random;
    prob_t p_qt_is_one;
    prob_t norm_coef;
    char curr_state[n+1];
    curr_state[n] = '\0';
    int curr_ct;

    htab_t *state_table = htab_init(n*n); //TODO: is optimal?
    
    for (unsigned long i=0; i < samples; i++) {
        norm_coef = 1;
        for (int j=0; j < n; j++) {
            curr_state[j] = NOT_MEASURED_CHAR;
        }

        for (int j=0; j < n; j++) {
            curr_ct = bits_to_measure[j];
            if (curr_ct == -1) {
                continue;
            }

            p_qt_is_one = measure(&circ, j, curr_state, n) * norm_coef * norm_coef;
            assert(p_qt_is_one <= (1 + EPSILON) && p_qt_is_one >= (0 - EPSILON));
            p_qt_is_one = (p_qt_is_one >= 1)? 1 : ((p_qt_is_one <= 0)? 0 : p_qt_is_one); // round
            random = (prob_t)rand() / RAND_MAX;
            if (random <= p_qt_is_one) {
                curr_state[curr_ct] = '1';
                assert(p_qt_is_one != 0);
                norm_coef *= sqrt(1/p_qt_is_one);
            }
            else {
                curr_state[curr_ct] = '0';
                assert(p_qt_is_one != 1);
                norm_coef *= sqrt(1/(1-p_qt_is_one));
            }
        }
        htab_m_lookup_add(state_table, curr_state);
    }
    htab_m_print_all(state_table, output);
    htab_m_free(state_table);
}

/* end of "sim.c" */
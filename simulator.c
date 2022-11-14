/**
 * Parses SliQSim examples (qasm files) and simulates them.
 * 
 * Examples from: https://github.com/NTU-ALComLab/SliQSim
 */

#include "custom_mtbdd.h"
#include "gates.h"

#include <ctype.h>
#include <stdbool.h>

#define CMD_MAX_LEN 10 // Max. supported length of qasm command
#define Q_ID_MAX_LEN 10 // Max. number of characters in qubit identificator
#define OUT_FILE "res" // Name of the output .dot file

extern uint32_t ltype_id;

/** 
 * Initialize for all qubit values 0.
 */
void circuit_init(MTBDD *a, const uint32_t n)
{
    BDDSET variables = mtbdd_set_empty();
    for (uint32_t i = 0; i < n; i++) {
        variables = mtbdd_set_add(variables, i);
    }

    cnum point = {.a = 1, .b = 0, .c = 0, .d = 0, .k = 0};
    uint8_t point_symbol[n];
    memset(point_symbol, 0, n*sizeof(uint8_t));
    MTBDD leaf  = mtbdd_makeleaf(ltype_id, (uint64_t) &point);
    *a = mtbdd_cube(variables, point_symbol, leaf);
}

/** 
 * Function for getting the next qubit index for the command on the given line.
 */
uint32_t get_q_num(FILE *in)
{
    int c;
    char num[Q_ID_MAX_LEN] = {0};
    unsigned long n;

    while ((c = fgetc(in)) != '[') {
        if (c == EOF) {
            error_exit("Invalid format - reached an unexpected end of file.");
        }
    }

    // Load number to string
    while ((c = fgetc(in)) != ']') {
        if (c == EOF) {
            error_exit("Invalid format - reached an unexpected end of file.");
        }
        else if (!isdigit(c)) {
            error_exit("Invalid format - not a valid qubit identifier.");
        }
        else if (strlen(num) + 1 < CMD_MAX_LEN) {
            int *temp = &c;
            strncat(num, (char*)temp, 1);
        }
        else {
            error_exit("Invalid command.");
        }
    }

    // Convert to integer value
    n = strtoul(num, NULL, 10);
    if (n > UINT32_MAX) {
        error_exit("Invalid format - not a valid qubit identifier.");
    }

    return ((uint32_t)n);
}

void sim_file(FILE *in, MTBDD *circ)
{
    
    int c;
    char cmd[CMD_MAX_LEN];
    bool init = false;

    while ((c = fgetc(in)) != EOF) {
        for (int i=0; i< CMD_MAX_LEN; i++) {
            cmd[i] = '\0';
        }

        while (isspace(c)) {
            c = fgetc(in);
        }

        if (c == EOF) {
            return;
        }

        // Skip one line comments
        if (c == '/') {
            if ((c = fgetc(in)) == '/') {
                while ((c = fgetc(in)) != '\n') {
                    if (c == EOF) {
                        return;
                    }
                }
                continue;
            }
            else {
                error_exit("Invalid command.");
            }
        }

        // Load the command
        do {
            if (c == EOF) {
                error_exit("Invalid format - reached an unexpected end of file.");
            }
            else if (strlen(cmd) + 1 < CMD_MAX_LEN) {
                int *temp = &c;
                strncat(cmd, (char *)temp, 1);
            }
            else {
                error_exit("Invalid command.");
            }
        } while (!isspace(c = fgetc(in)));
        if (c == EOF) {
            error_exit("Invalid format - reached an unexpected end of file.");
        }

        //TODO: pridat vic testu
        
        //TOD0: doxygen comments
        //TOD0: projit kod vsechny ?? a !todo
        //TOD0: print pravdepodobnosti:
            /*
             * 1. my op probability print do extra filu - problem: jiny typ stromu (floaty)
             * 2. dodelat dalsi output file: strom vs tabulka?
             */

        // Identify the command
        if (strcmp(cmd, "OPENQASM") == 0) {}
        else if (strcmp(cmd, "include") == 0) {}
        else if (strcmp(cmd, "creg") == 0) {}
        else if (strcmp(cmd, "qreg") == 0) {
            uint32_t n = get_q_num(in);
            circuit_init(circ, n);
            init = true;
        }
        else if (init) {
            if (strcmp(cmd, "measure") == 0) {
                //TODO: dodelat
            }
            else if (strcmp(cmd, "x") == 0) {
                uint32_t qt = get_q_num(in);
                *circ = gate_x(circ, qt);
            }
            else if (strcmp(cmd, "y") == 0) {
                uint32_t qt = get_q_num(in);
                *circ = gate_y(circ, qt);
            }
            else if (strcmp(cmd, "z") == 0) {
                uint32_t qt = get_q_num(in);
                *circ = gate_z(circ, qt);
            }
            else if (strcmp(cmd, "h") == 0) {
                uint32_t qt = get_q_num(in);
                *circ = gate_h(circ, qt);
            }
            else if (strcmp(cmd, "s") == 0) {
                uint32_t qt = get_q_num(in);
                *circ = gate_s(circ, qt);
            }
            else if (strcmp(cmd, "t") == 0) {
                uint32_t qt = get_q_num(in);
                *circ = gate_t(circ, qt);
            }
            else if (strcmp(cmd, "rx(pi/2)") == 0) {
                uint32_t qt = get_q_num(in);
                *circ = gate_rx_pihalf(circ, qt);
            }
            else if (strcmp(cmd, "ry(pi/2)") == 0) {
                uint32_t qt = get_q_num(in);
                *circ = gate_ry_pihalf(circ, qt);
            }
            else if (strcmp(cmd, "cx") == 0) {
                uint32_t qc = get_q_num(in);
                uint32_t qt = get_q_num(in);
                *circ = gate_cnot(circ, qt, qc);
            }
            else if (strcmp(cmd, "cz") == 0) {
                uint32_t qc = get_q_num(in);
                uint32_t qt = get_q_num(in);
                *circ = gate_cz(circ, qt, qc);
            }
            else if (strcmp(cmd, "ccx") == 0) {
                uint32_t qc1 = get_q_num(in);
                uint32_t qc2 = get_q_num(in);
                uint32_t qt = get_q_num(in);
                *circ = gate_toffoli(circ, qt, qc1, qc2);
            }
            else if (strcmp(cmd, "cswap") == 0) {
                uint32_t qc = get_q_num(in);
                uint32_t qt1 = get_q_num(in);
                uint32_t qt2 = get_q_num(in);
                *circ = gate_fredkin(circ, qt1, qt2, qc);
            }
            else {
                error_exit("Invalid command.");
            }
        }
        else {
            error_exit("Circuit not initialized.");
        }

        // Skip all remaining characters on the currently read line
        while ((c = fgetc(in)) != ';') {
            if (c == EOF) {
                error_exit("Invalid format - reached an unexpected end of file.");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *input;
    if (argc == 1){
        input = stdin;
    }
    else if (argc == 2){
        input = fopen(argv[1], "r");
    }
    else {
        error_exit("Invalid number of arguments.");
    }
    init_sylvan();
    init_my_leaf();
    FILE* out = fopen(OUT_FILE".dot", "w");
    MTBDD circ;

    sim_file(input, &circ);

    mtbdd_fprintdot(out, circ);
    
    fclose(out);
    if (argc == 2){
        fclose(input);
    }
    sylvan_quit();
    lace_stop();

    return 0;
}

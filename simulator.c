/**
 * Parses SliQSim examples (qasm files) and simulates them.
 * 
 * Examples from: https://github.com/NTU-ALComLab/SliQSim
 */

#include "custom_mtbdd.h"
#include "gates.h"

#include <ctype.h>

#define CMD_MAX_LEN 10
#define Q_ID_MAX_LEN 20

extern uint32_t ltype_id;
/* TEST */
MTBDD circ;
FILE* f_orig;
/* TEST */

#define ERROR_TEXT "ERROR: "
void error_exit(const char *error)
{
    // create error message format
    unsigned error_len = strlen(error) + strlen(ERROR_TEXT);
    char msg[error_len + 1];
    strcpy(msg, ERROR_TEXT);
    strcat(msg, error);

    fprintf(stderr, "%s\n",msg);
    exit(1);
}

/** 
 * Initialize for all qubit values 0.
 */
void circuit_init(MTBDD *a, const uint32_t n)
{
    BDDSET variables = mtbdd_set_empty();
    for (uint32_t i = 0; i < n; i++) {
        variables = mtbdd_set_add(variables, i);
    }

    cnum point = {.a = 1, .b = 0, .c = 0, .d = 0, .k = 1}; //FIXME: check
    uint8_t point_symbol[n];
    memset(point_symbol, 0, n*sizeof(uint8_t));
    MTBDD leaf  = mtbdd_makeleaf(ltype_id, (uint64_t) &point);
    *a = mtbdd_cube(variables, point_symbol, leaf);
}

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
    char *end;
    n = strtoul(num, &end, 10);
    if (*end != '\0') {
        //FIXME: ma tu byt error?
        error_exit("Invalid format - not a valid qubit identifier.");
    }
    if (n > UINT32_MAX) {
        error_exit("Invalid format - not a valid qubit identifier.");
    }

    return ((uint32_t)n);
}

void sim_file(FILE *in)
{
    int c;
    char cmd[CMD_MAX_LEN];
    //MTBDD circ; //TODO: kam toto patri
/* TEST */
    FILE* out = fopen("out.txt", "w");
/* TEST */

    while ((c = fgetc(in)) != EOF) {
        for (int i=0; i< CMD_MAX_LEN; i++) {
            cmd[i] = '\0';
        }

        while (isspace(c)) {
            c = fgetc(in);
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

        //printf("---cmd=%s---\n",cmd);/* TEST */

        // Identify the command
        if (strcmp(cmd, "OPENQASM") == 0) {fprintf(out,"OPENQASM\n")/*TEST*/;}
        else if (strcmp(cmd, "include") == 0) {fprintf(out,"include\n")/*TEST*/;}
        else if (strcmp(cmd, "creg") == 0) {fprintf(out,"creg\n")/*TEST*/;} //FIXME: zjistit co to znamena
        else if (strcmp(cmd, "qreg") == 0) {
            uint32_t n = get_q_num(in);
            fprintf(out,"init %d qubits\n", n)/*TEST*/;
            circuit_init(&circ, n);
/* TEST */ mtbdd_fprintdot(f_orig, circ);
        }
        else if (strcmp(cmd, "measure") == 0) {
            //TODO: jak na print?
        }
        else if (strcmp(cmd, "x") == 0) {
            uint32_t qt = get_q_num(in);
            fprintf(out, "x[%d]\n", qt)/*TEST*/;
            gate_x(&circ, qt);
        }
        else if (strcmp(cmd, "y") == 0) {
            uint32_t qt = get_q_num(in);
            gate_y(&circ, qt);
        }
        else if (strcmp(cmd, "z") == 0) {
            uint32_t qt = get_q_num(in);
            gate_z(&circ, qt);
        }
        else if (strcmp(cmd, "h") == 0) {
            uint32_t qt = get_q_num(in);
            fprintf(out, "h[%d]\n", qt)/*TEST*/;
            gate_h(&circ, qt);
        }
        else if (strcmp(cmd, "s") == 0) {
            uint32_t qt = get_q_num(in);
            gate_s(&circ, qt);
        }
        else if (strcmp(cmd, "t") == 0) {
            uint32_t qt = get_q_num(in);
            gate_t(&circ, qt);
        }
        else if (strcmp(cmd, "rx(pi/2)") == 0) {
            uint32_t qt = get_q_num(in);
            gate_rx_pihalf(&circ, qt);
        }
        else if (strcmp(cmd, "ry(pi/2)") == 0) {
            uint32_t qt = get_q_num(in);
            gate_ry_pihalf(&circ, qt);
        }
        else if (strcmp(cmd, "cx") == 0) {
            uint32_t qt = get_q_num(in);
            uint32_t qc = get_q_num(in);
            fprintf(out, "cx[%d][c=%d]\n", qt, qc)/*TEST*/;
            gate_cnot(&circ, qt, qc);
        }
        else if (strcmp(cmd, "cz") == 0) {
            uint32_t qt = get_q_num(in);
            uint32_t qc = get_q_num(in);
            gate_cz(&circ, qt, qc);
        }
        /* TODO: Toffoli & Fredkin 
        else if (strcmp(cmd, "") == 0) {

        }
        else if (strcmp(cmd, "") == 0) {

        }
        */
        else {
            error_exit("Invalid command.");
        }
        
        // Skip all remaining characters on the currently read line
        while ((c = fgetc(in)) != ';') {
            if (c == EOF) {
                error_exit("Invalid format - reached an unexpected end of file.");
            }
        }
    }

/* TEST */
    fclose(out);
/* TEST */
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

/*TEST*/
    f_orig = fopen("orig.dot", "w");
    FILE* f_res = fopen("res.dot", "w");
/*TEST*/

    sim_file(input);

/*TEST*/
    mtbdd_fprintdot(f_res, circ);
    
    fclose(f_orig);
    fclose(f_res);
/*TEST*/


    if (argc == 2){
        fclose(input);
    }
    sylvan_quit();
    lace_stop();

    return 0;
}

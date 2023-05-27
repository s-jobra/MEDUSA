#include <time.h>
#include "sim.h"
#include "error.h"

#define OUT_FILE "res" // Name of the output .dot file

int main(int argc, char *argv[])
{
    
    FILE *input = stdin;
    bool time = false;

    switch (argc) {
        case 1:
            break;
        case 2:
            if(strcmp(argv[1], "--time") == 0) {
                time = true;
            }
            else {
                input = fopen(argv[1], "r");
                if (input == NULL) {
                    error_exit("Invalid input file.");
                }
            }
            break;
        case 3:
            if(strcmp(argv[1], "--time") == 0) {
                time = true;
                input = fopen(argv[2], "r");
                if (input == NULL) {
                    error_exit("Invalid input file.");
                }
            }
            else if (strcmp(argv[2], "--time") == 0) {
                time = true;
                input = fopen(argv[1], "r");
                if (input == NULL) {
                    error_exit("Invalid input file.");
                }
            }
            else {
                error_exit("Unknown argument.");
            }
            break;
        default:
            error_exit("Invalid number of arguments.");
            break;
    }

    init_sylvan();
    init_my_leaf();

    FILE* out = fopen(OUT_FILE".dot", "w");
    if (out == NULL) {
        error_exit("Cannot open output file.");
    }
    MTBDD circ;
    int *bits_to_measure;
    bool is_measure = false;
    int n_qubits;

    struct timespec t_start, t_finish;
    double t_el;
    clock_gettime(CLOCK_MONOTONIC, &t_start); // Start the timer

    sim_file(input, &circ, &n_qubits, &bits_to_measure, &is_measure);

    clock_gettime(CLOCK_MONOTONIC, &t_finish); // End the timer

    if (is_measure) { //TODO: separate function
        int samples=10; //TODO: proper input
        printf("Sampled results:\n");

        prob_t random;
        prob_t p_qt_is_one;
        prob_t norm_coef;
        char curr_state[n_qubits];
        int curr_ct;
        
        for (int i=0; i < samples; i++) {
            norm_coef = 1;
            for (int i=0; i < n_qubits; i++) {
                curr_state[i] = 'x';
            }

            for (int j=0; j < n_qubits; j++) {
                curr_ct = bits_to_measure[j];
                if (curr_ct == -1) {
                    continue;
                }

                p_qt_is_one = measure(&circ, j, curr_state, n_qubits) * norm_coef;
                random = (prob_t)rand() / RAND_MAX;
                if (random <= p_qt_is_one) {
                    curr_state[curr_ct] = '1';
                    norm_coef *= sqrt(1/p_qt_is_one);
                }
                else {
                    curr_state[curr_ct] = '0';
                    norm_coef *= sqrt(1/(1-p_qt_is_one));
                }
            }
            
            //FIXME: temporary output
            for (int i=0; i < n_qubits; i++) {
                printf(curr_state[i]);
            }
            printf("\n");
        }
    }
    
    mtbdd_fprintdot(out, circ);
    mpz_clear(c_k);

    fclose(out);
    if (argc == 2){
        fclose(input);
    }

    sylvan_quit();
    lace_stop();

    t_el = t_finish.tv_sec - t_start.tv_sec + (t_finish.tv_nsec - t_start.tv_nsec) * 1.0e-9;

    if (time) {
        printf("Time=%.3gs\n", t_el);
    }

    return 0;
}
#include <time.h>
#include "sim.h"
#include "error.h"

#define OUT_FILE "res" // Name of the output .dot file

int main(int argc, char *argv[])
{
    FILE *input = stdin;
    FILE *measure_output = stdout;
    bool time = false;
    int opt;
    unsigned long samples=1024;

    //TODO: add help + long options
    char *endptr;
    while((opt = getopt(argc, argv, "tf:s:m:")) != -1) {
        switch(opt) {
            case 't':
                time = true;
                break; 
            case 'f':
                input = fopen(optarg, "r");
                if (input == NULL) {
                    error_exit("Invalid input file.");
                }
                break;
            case 's':
                samples = strtoul(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    error_exit("Invalid number of samples.");
                }
                break;
            case 'm':
                measure_output = fopen(optarg, "r");
                if (measure_output == NULL) {
                    error_exit("Invalid output file.");
                }
                break;
            case '?':
                error_exit("Unknown argument.");
        }
    }

    init_sylvan();
    init_my_leaf();

    FILE *out = fopen(OUT_FILE".dot", "w");
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

    if (is_measure) {
        measure_all(samples, measure_output, circ, n_qubits, bits_to_measure);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_finish); // End the timer
    
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
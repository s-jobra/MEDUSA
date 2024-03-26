#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <sys/resource.h>
#include "sim.h"
#include "mtbdd_out.h"
#include "error.h"

#define OUT_FILE "res"                    // Name of the output .dot file
#define LONG_NUMS_OUT_FILE "res-vars.txt" // Name of the output file with the large numbers
#define LONG_NUMS_MAP_INIT_SIZE 5         // Initial size of array for the separate output of large numbers
#define HELP_MSG \
" Usage: MEDUSA [options] \n\
\n\
 Options with no argument:\n\
 --help,     -h          show this message\n\
 --info,     -i          measure the simulation runtime and peak memory usage\n\
 --symbolic, -s          perform symbolic simulation if possible\n\
 \n\
 Options with a required argument:\n\
 --file,     -f          specify the input QASM file (default STDIN)\n\
 --nsamples, -n          specify the number of samples used for measurement (default 1024)\n\
 \n\
 Options with an optional argument:\n\
 --measure,  -m          perform the measure operations encountered in the circuit, \n\
                         optional arg specifies the file for saving the measurement result (default STDOUT)\n\
 \n\
 The MTBDD result is saved in the file 'res.dot'.\n\
 The evaluation of variables for large numbers is saved (if necessary) in 'res-vars.txt'.\n"

/**
 * Returns the peak physical memory usage of the process in kilobytes. For an unsupported OS returns -1.
 */
static long get_peak_mem()
{
    long peak = 0;
    #if defined(__unix__) || defined(__APPLE__)
        struct rusage rs_usage;
        if (getrusage(RUSAGE_SELF, &rs_usage) == 0) {
            peak = rs_usage.ru_maxrss;
        }
    #else
        // Unknown OS
        peak = -1;
    #endif
    return peak;
}

int main(int argc, char *argv[])
{
    FILE *input = stdin;
    FILE *measure_output = stdout;
    bool opt_infile = false;
    bool opt_info = false;
    bool opt_measure = false;
    bool opt_symbolic = false;
    unsigned long samples = 1024;
    
    int opt;
    static struct option long_options[] = {
        {"help",     no_argument,        0, 'h'},
        {"info",     no_argument,        0, 'i'},
        {"file",     required_argument,  0, 'f'},
        {"measure",  optional_argument,  0, 'm'},
        {"nsamples", required_argument,  0, 'n'},
        {"symbolic", no_argument,        0, 's'},
        {0, 0, 0, 0}
    };
    char *endptr;
    while((opt = getopt_long(argc, argv, "hif:m::n:s", long_options, 0)) != -1) {
        switch(opt) {
            case 'h':
                printf("%s\n", HELP_MSG);
                exit(0);
            case 'i':
                opt_info = true;
                break; 
            case 'f':
                opt_infile = true;
                input = fopen(optarg, "r");
                if (input == NULL) {
                    error_exit("Invalid input file '%s'.\n", optarg);
                }
                break;
            case 'm':
                opt_measure = true;
                if (!optarg && optind < argc && argv[optind][0] != '-') {
                    optarg = argv[optind++];
                    measure_output = fopen(optarg, "w");
                    if (measure_output == NULL) {
                        error_exit("Invalid output file '%s'.\n", optarg);
                    }
                }
                break;
            case 'n':
                samples = strtoul(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    error_exit("Invalid number of samples.\n");
                }
                break;
            case 's':
                opt_symbolic = true;
                break;
            case '?':
                exit(1); // error msg already printed by getopt_long
        }
    }

    // Init:
    init_sylvan();
    init_my_leaf();
    if (opt_symbolic) {
        init_sylvan_symb();
    }
    FILE *out = fopen(OUT_FILE".dot", "w");
    if (out == NULL) {
        error_exit("Cannot open the output file.\n");
    }
    MTBDD circ;
    int *bits_to_measure;
    bool is_measure = false;
    int n_qubits;

    // Sim:
    struct timespec t_start, t_finish;
    double t_el;
    clock_gettime(CLOCK_MONOTONIC, &t_start); // Start the timer

    sim_file(input, &circ, &n_qubits, &bits_to_measure, &is_measure, opt_symbolic);

    if (opt_measure && is_measure) {
        measure_all(samples, measure_output, circ, n_qubits, bits_to_measure);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_finish); // End the timer
    
    // Output:
    lnum_map_init(LONG_NUMS_MAP_INIT_SIZE);
    mtbdd_fprintdot(out, circ);
    // Check if there are any large numbers outputted only as variables in the .dot file
    if (!lnum_map_is_empty()) {
        FILE *lnums_out = fopen(LONG_NUMS_OUT_FILE, "w");
        if (lnums_out == NULL) {
            error_exit("Cannot open the output file for the separate output for large numbers.\n");
        }
        lnum_map_print(lnums_out);
        fclose(lnums_out);
    }
    lnum_map_clear();

    t_el = t_finish.tv_sec - t_start.tv_sec + (t_finish.tv_nsec - t_start.tv_nsec) * 1.0e-9;
    if (opt_info) {
        printf("Time=%.3gs\n", t_el);
        #if defined(__unix__) || defined(__APPLE__)
            printf("Peak Memory Usage=%ldkB\n", get_peak_mem());
        #else
            printf("Peak Memory Usage not supported for this OS.\n");
        #endif
    }

    // Finish:
    circuit_delete(&circ);
    stop_sylvan();
    fclose(out);
    if (opt_infile) {
        fclose(input);
    }

    return 0;
}
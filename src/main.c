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

    clock_t t_start = clock(); // Start the timer

    sim_file(input, &circ);

    clock_t t_end = clock(); // End the timer

    mtbdd_fprintdot(out, circ);

    fclose(out);
    if (argc == 2){
        fclose(input);
    }
    sylvan_quit();
    lace_stop();

    if (time) {
        printf("Time=%.3gs\n", (double)(t_end - t_start)/CLOCKS_PER_SEC);
    }

    return 0;
}
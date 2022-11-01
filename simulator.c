#include "custom_mtbdd.h"
#include "gates.h"

/**
 * Parses SliQSim examples (qasm files) and simulates them.
 * 
 * Examples from: https://github.com/NTU-ALComLab/SliQSim
 */




int main(int argc, char *argv[])
{
    // Get input
    FILE *input;
    if (argc == 1){
        input = stdin;
    }
    else if (argc == 2){
        input = fopen(argv[1], "r");
    }
    else {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }

    


    return 0;
}
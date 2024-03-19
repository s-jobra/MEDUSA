#!/bin/bash
export LC_ALL=C.UTF-8

# It is assumed that the script is run from the repository's home folder.

#####################################################################################
# Constants:

# Output files
FILE_OUT_MEASURE="measure.csv"
FILE_OUT="no-measure.csv"

# Exec settings
MEDUSA_EXEC="./sim"
SLIQSIM_EXEC="../SliQSim-no-out/SliQSim"
SLIQSIM_EXEC_MSR="../SliQSim/SliQSim"

MEDUSA_BASE_OPT="-i"
MEDUSA_SYMB_OPT="-s"
MEDUSA_MEASURE_OPT="-m"
SLIQSIM_BASE_OPT="--print_info"
SLIQSIM_OPT="--type 1"
SLIQSIM_MEASURE_OPT="--type 0 --shots 1024"

# Benchmark directories
BENCH_NO_MEASURE="./benchmarks/no-measure/"
BENCH_MEASURE="./benchmarks/measure/"

# Measurement settings
REPS=1
TIMEOUT="timeout 1h"   # for no timeout: ""

# Output settings
SEP=","
NO_RUN="---"
FAILED="Failed"

#####################################################################################
# Functions:

sim_file() {
    medusa_symb_time_sum=0.0
    medusa_time_sum=0.0
    sliqsim_time_sum=0.0

    medusa_symb_mem_sum=0.0
    medusa_mem_sum=0.0
    sliqsim_mem_sum=0.0
    
    #TODO: change MEDUSA call to 1 function call, change vars to arrays

    # MEDUSA SYMBOLIC
    if [[ $medusa_symb_fail = false ]]; then
        for i in $(eval echo "{1..$REPS}"); do
            medusa_symb_output=$($TIMEOUT $MEDUSA_EXEC $medusa_run_symb_opt <$file /dev/null 2>/dev/null | grep -E 'Time=|Peak Memory Usage=')
            medusa_symb_time_cur=$(echo "$medusa_symb_output" | grep -oP '(?<=Time=)[0-9.]+' | awk '{printf "%.4f", $1}')
            medusa_symb_mem_cur=$(echo "$medusa_symb_output" | grep -oP '(?<=Peak Memory Usage=)[0-9]+' | awk '{printf "%.2f", $1 / 1024}')
            if [[ -z $medusa_symb_time_cur ]]; then
                medusa_symb_time_avg=$FAILED
                medusa_symb_mem_avg=$FAILED
                medusa_symb_fail=true
                break;
            else
                medusa_symb_time_sum=$(echo "scale=4; $medusa_symb_time_sum + $medusa_symb_time_cur" | bc)
                medusa_symb_mem_sum=$(echo "scale=2; $medusa_symb_mem_sum + $medusa_symb_mem_cur" | bc)
            fi
        done
        if [[ $medusa_symb_fail = false ]]; then
            medusa_symb_time_avg=$(echo "scale=4; $medusa_symb_time_sum / $REPS.0" | bc)
            medusa_symb_mem_avg=$(echo "scale=2; $medusa_symb_mem_sum / $REPS.0" | bc)
        fi
    else
        medusa_symb_time_avg=$NO_RUN
        medusa_symb_mem_avg=$NO_RUN
    fi

    # Change to file without loops for normal simulation if in loop directory
    if [[ $is_loop = true ]]; then
        directory=$(dirname "$file")
        filename=$(basename "$file")
        new_filename="NL_$filename"

        if [ -f "$directory/$new_filename" ]; then
            file="$directory/$new_filename"
        else
            echo "Missing file $directory/$new_filename" >&2
        fi
    fi

    # MEDUSA NORMAL
    if [[ $medusa_fail = false ]]; then
        for i in $(eval echo "{1..$REPS}"); do
            medusa_output=$($TIMEOUT $MEDUSA_EXEC $medusa_run_opt <$file /dev/null 2>/dev/null | grep -E 'Time=|Peak Memory Usage=')
            medusa_time_cur=$(echo "$medusa_output" | grep -oP '(?<=Time=)[0-9.]+' | awk '{printf "%.4f", $1}')
            medusa_mem_cur=$(echo "$medusa_output" | grep -oP '(?<=Peak Memory Usage=)[0-9]+' | awk '{printf "%.2f", $1 / 1024}')
            if [[ -z $medusa_time_cur ]]; then
                medusa_time_avg=$FAILED
                medusa_mem_avg=$FAILED
                medusa_fail=true
                break;
            else
                medusa_time_sum=$(echo "scale=4; $medusa_time_sum + $medusa_time_cur" | bc)
                medusa_mem_sum=$(echo "scale=2; $medusa_mem_sum + $medusa_mem_cur" | bc)
            fi
        done
        if [[ $medusa_fail = false ]]; then
            medusa_time_avg=$(echo "scale=4; $medusa_time_sum / $REPS.0" | bc)
            medusa_mem_avg=$(echo "scale=2; $medusa_mem_sum / $REPS.0" | bc)
        fi
    else
        medusa_time_avg=$NO_RUN
        medusa_mem_avg=$NO_RUN
    fi

    # SLIQSIM NORMAL
    if [[ $sliqsim_fail = false ]]; then
        for i in $(eval echo "{1..$REPS}"); do
            sliqsim_output=$($TIMEOUT $sliqsim_run_exec --sim_qasm $file $sliqsim_run_opt 2>/dev/null | \
                             grep -E 'Runtime:|Peak memory usage:')
            sliqsim_time_cur=$(echo "$sliqsim_output" | grep -oP '(?<=Runtime: )[0-9.]+' | awk '{printf "%.4f", $1}')
            sliqsim_mem_cur=$(echo "$sliqsim_output" | grep -oP '(?<=Peak memory usage: )[0-9]+' | awk '{printf "%.2f", $1 / (1024 * 1024)}')
            if [[ -z $sliqsim_time_cur ]]; then
                sliqsim_time_avg=$FAILED
                sliqsim_mem_avg=$FAILED
                sliqsim_fail=true
                break;
            else
                sliqsim_time_sum=$(echo "scale=4; $sliqsim_time_sum + $sliqsim_time_cur" | bc)
                sliqsim_mem_sum=$(echo "scale=2; $sliqsim_mem_sum + $sliqsim_mem_cur" | bc)
            fi
        done
        if [[ $sliqsim_fail = false ]]; then
            sliqsim_time_avg=$(echo "scale=4; $sliqsim_time_sum / $REPS.0" | bc)
            sliqsim_mem_avg=$(echo "scale=2; $sliqsim_mem_sum / $REPS.0" | bc)
        fi
    else
        sliqsim_time_avg=$NO_RUN
        sliqsim_mem_avg=$NO_RUN
    fi
}

# Expects the file path to the test as the first argument and if it is symbolic circuit as the second argument
run_benchmark_file() {
    local file=$1
    local is_loop=$2

    benchmark_name=$(echo "$file" | cut -d'/' -f4-)
    sim_file
    printf "%s$SEP%s$SEP%s$SEP%s$SEP%s$SEP%s$SEP%s\n" "${benchmark_name%.qasm}" "$medusa_symb_time_avg" "$medusa_symb_mem_avg" \
           "$medusa_time_avg" "$medusa_mem_avg" "$sliqsim_time_avg" "$sliqsim_mem_avg" \
           >> "$file_out"
}

run_benchmarks() {
    # Init
    if [[ $is_measure = true ]]; then
        benchmarks_fd=$BENCH_MEASURE
        medusa_run_symb_opt="$MEDUSA_BASE_OPT $MEDUSA_SYMB_OPT $MEDUSA_MEASURE_OPT"
        medusa_run_opt="$MEDUSA_BASE_OPT $MEDUSA_MEASURE_OPT"
        sliqsim_run_opt="$SLIQSIM_BASE_OPT $SLIQSIM_MEASURE_OPT"
        sliqsim_run_exec=$SLIQSIM_EXEC_MSR
        file_out=$FILE_OUT_MEASURE
    else
        benchmarks_fd=$BENCH_NO_MEASURE
        medusa_run_symb_opt="$MEDUSA_BASE_OPT $MEDUSA_SYMB_OPT"
        medusa_run_opt=$MEDUSA_BASE_OPT
        sliqsim_run_opt="$SLIQSIM_BASE_OPT $SLIQSIM_OPT"
        sliqsim_run_exec=$SLIQSIM_EXEC
        file_out=$FILE_OUT
    fi

    printf "%s$SEP%s$SEP%s$SEP%s$SEP%s$SEP%s$SEP%s\n" "Circuit" "MEDUSA symbolic t" "MEDUSA symbolic mem" "MEDUSA t" \
           "MEDUSA mem" "SliQSim t" "SliQSim mem" \
           >> "$file_out"

    for folder in "$benchmarks_fd"*/; do
        folder_name=$(basename "$folder")

        # Initialize the flags for crashes and exceeding the timeout
        medusa_symb_fail=false
        medusa_fail=false
        sliqsim_fail=false

        if [[ ! "$folder_name" =~ ^LP- ]]; then
            # Non loop circuits
            for file in "$folder"*.qasm; do
                medusa_symb_fail=true # So symbolic simulation is not run
                if [ -f "$file" ]; then
                    run_benchmark_file $file false
                fi
                # Random and RevLib - nonlinear progression in simulation time -> wait for TO on every benchmark (restart flags)
                if [[ ( "$folder_name" = "Random" ) || ( "$folder_name" = "RevLib" ) || ( "$folder_name" = "Feynman" ) ]]; then
                    medusa_symb_fail=false
                    medusa_fail=false
                    sliqsim_fail=false
                fi
            done
        else
            # Circuits with a loop variant
            for file in "$folder"[0-9]*.qasm; do
                # To avoid an invalid iteration when no file matches the criteria
                if [ -f "$file" ]; then
                    run_benchmark_file $file true
                fi
            done
        fi
    done
}

#####################################################################################
# Output:
is_measure=false # global so it can be read from all the functions
run_benchmarks

is_measure=true
run_benchmarks
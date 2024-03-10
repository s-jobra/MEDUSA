#!/bin/bash
export LC_ALL=C.UTF-8

# It is assumed that the script is run from the repository's home folder.

#####################################################################################
# Constants:

# Exec settings
MEDUSA_EXEC="./sim"
SLIQSIM_EXEC="../SliQSim-no-out/SliQSim"
SLIQSIM_EXEC_MSR="../SliQSim/SliQSim"

MEDUSA_BASE_OPT="-t"
MEDUSA_SYMB_OPT="-s"
MEDUSA_MEASURE_OPT="-m"
SLIQSIM_BASE_OPT="--print_info"
SLIQSIM_OPT="--type 1"
SLIQSIM_MEASURE_OPT="--type 0 --shots 1024"

#Benchmark directories
BENCH_NO_MEASURE="./benchmarks/no-measure/"
BENCH_MEASURE="./benchmarks/measure/"

# Measurement settings
REPS=10
TIMEOUT="timeout 10h"   # for no timeout: ""

# Output color settings
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
NC=$(tput sgr0)

# Other output constants
SEP="============================================================================================================="
NO_RUN=" --- "

#####################################################################################
# Functions:

sim_file() {
    medusa_symb_time_sum=0.0
    medusa_time_sum=0.0
    sliqsim_time_sum=0.0
    
    #TODO: change MEDUSA call to 1 function call
    # MEDUSA SYMBOLIC
    if [[ $medusa_symb_fail = false ]]; then
        for i in $(eval echo "{1..$REPS}"); do
            medusa_symb_time_cur=$( $TIMEOUT $MEDUSA_EXEC $medusa_run_symb_opt <$file /dev/null 2>/dev/null | awk '{gsub(/Time=/,""); printf "%.4f \n", $1}')
            if [[ -z $medusa_symb_time_cur ]]; then
                medusa_symb_time_avg="${RED}Failed$NC"
                medusa_symb_fail=true
                break;
            else
                medusa_symb_time_sum=$(echo "scale=4; $medusa_symb_time_sum + $medusa_symb_time_cur" | bc)
            fi
        done
        if [[ $medusa_symb_fail = false ]]; then
            medusa_symb_time_avg=$(echo "scale=4; $medusa_symb_time_sum / $REPS.0" | bc)
        fi
    else
        medusa_symb_time_avg=$NO_RUN
    fi

    # Change to file without loops for normal simulation if in loop directory
    if [[ $is_loop = true ]]; then
        directory=$(dirname "$file")
        filename=$(basename "$file")
        new_filename="NL_$filename"

        file="$directory/$new_filename"
    fi

    # MEDUSA NORMAL
    if [[ $medusa_fail = false ]]; then
        for i in $(eval echo "{1..$REPS}"); do
            medusa_time_cur=$( $TIMEOUT $MEDUSA_EXEC $medusa_run_opt <$file /dev/null 2>/dev/null | awk '{gsub(/Time=/,""); printf "%.4f \n", $1}')
            if [[ -z $medusa_time_cur ]]; then
                medusa_time_avg="${RED}Failed$NC"
                medusa_fail=true
                break;
            else
                medusa_time_sum=$(echo "scale=4; $medusa_time_sum + $medusa_time_cur" | bc)
            fi
        done
        if [[ $medusa_fail = false ]]; then
            medusa_time_avg=$(echo "scale=4; $medusa_time_sum / $REPS.0" | bc)
        fi
    else
        medusa_time_avg=$NO_RUN
    fi

    # SLIQSIM NORMAL
    if [[ $sliqsim_fail = false ]]; then
        for i in $(eval echo "{1..$REPS}"); do
            sliqsim_time_cur="$( $TIMEOUT $sliqsim_run_exec --sim_qasm $file $sliqsim_run_opt 2>/dev/null | \
                            awk -F: '/Runtime:/{gsub(/ seconds/,""); printf "%.4fs \n", $2}')"
            sliqsim_time_cur=${sliqsim_time_cur%s*}
            if [[ -z $sliqsim_time_cur ]]; then
                sliqsim_time_avg="${RED}Failed$NC"
                sliqsim_fail=true
                break;
            else
                sliqsim_time_sum=$(echo "scale=4; $sliqsim_time_sum + $sliqsim_time_cur" | bc)
            fi
        done
        if [[ $sliqsim_fail = false ]]; then
            sliqsim_time_avg=$(echo "scale=4; $sliqsim_time_sum / $REPS.0" | bc)
        fi
    else
        sliqsim_time_avg=$NO_RUN
    fi
}

# Compares the given two strings and colours them appropriately based on their value
cmp_two() {
    local -n a=$1
    local -n b=$2

    if (( $(echo "$a < $b" | bc -l) )); then
        a+="s"
        b+="s"
        a="${GREEN}${a}$NC"
        b="${YELLOW}${b}$NC"
    elif (( $(echo "$a > $b" | bc -l) )); then
        a+="s"
        b+="s"
        b="${GREEN}${b}$NC"
        a="${YELLOW}${a}$NC"
    else
        a+="s"
        b+="s"
        b="${GREEN}${b}$NC"
        a="${GREEN}${a}$NC"
    fi
}

# Compares the given three strings and colours them appropriately based on their value
cmp_three() {
    local -n a=$1
    local -n b=$2
    local -n c=$3

    min_value=$(echo -e "$a\n$b\n$c" | sort -n | head -n 1)

    if (( $(echo "$a == $min_value" | bc -l) )); then
        a+="s"
        a="${GREEN}${a}$NC"
    else
        a+="s"
        a="${YELLOW}${a}$NC"
    fi

    if (( $(echo "$b == $min_value" | bc -l) )); then
        b+="s"
        b="${GREEN}${b}$NC"
    else
        b+="s"
        b="${YELLOW}${b}$NC"
    fi

    if (( $(echo "$c == $min_value" | bc -l) )); then
        c+="s"
        c="${GREEN}${c}$NC"
    else
        c+="s"
        c="${YELLOW}${c}$NC"
    fi
}

colour_results() {
    # Colour the results (if fail, no colour change)
    if [[ ( $medusa_symb_fail = false ) && ( $medusa_fail = false ) && ( $sliqsim_fail = false ) ]]; then
        cmp_three medusa_symb_time_avg medusa_time_avg sliqsim_time_avg
    elif [[ ( $medusa_symb_fail = true ) && ( $medusa_fail = false ) && ( $sliqsim_fail = false ) ]]; then
        cmp_two medusa_time_avg sliqsim_time_avg
    elif [[ ( $medusa_symb_fail = false ) && ( $medusa_fail = false ) && ( $sliqsim_fail = true ) ]]; then
        cmp_two medusa_symb_time_avg medusa_time_avg
    elif [[ ( $medusa_symb_fail = false ) && ( $medusa_fail = true ) && ( $sliqsim_fail = false ) ]]; then
        cmp_two medusa_symb_time_avg sliqsim_time_avg
    elif [[ ( $medusa_symb_fail = true ) && ( $medusa_fail = true ) && ( $sliqsim_fail = false ) ]]; then
        sliqsim_time_avg+="s"
        sliqsim_time_avg="${GREEN}${sliqsim_time_avg}$NC"
    elif [[ ( $medusa_symb_fail = true ) && ( $medusa_fail = false ) && ( $sliqsim_fail = true ) ]]; then
        medusa_time_avg+="s"
        medusa_time_avg="${GREEN}${medusa_time_avg}$NC"
    elif [[ ( $medusa_symb_fail = false ) && ( $medusa_fail = true ) && ( $sliqsim_fail = true ) ]]; then
        medusa_symb_time_avg+="s"
        medusa_symb_time_avg="${GREEN}${medusa_symb_time_avg}$NC"
    fi
}

# Expects the file path to the test as the first argument and if it is symbolic circuit as the second argument
run_benchmark_file() {
    local file=$1
    local is_loop=$2

    benchmark_name=$(echo "$file" | cut -d'/' -f4-)
    sim_file
    colour_results
    printf '%-35s %-20s %-31s %s\n' "${benchmark_name%.qasm}" "$medusa_symb_time_avg" "$medusa_time_avg" "$sliqsim_time_avg"
}

run_benchmarks() {
    printf "$SEP\n"
    printf '%-35s %-20s %-20s %s\n' "File" "MEDUSA symbolic" "MEDUSA" "SliQSim"
    printf "$SEP\n"

    if [[ $is_measure = true ]]; then
        benchmarks_fd=$BENCH_MEASURE
        medusa_run_symb_opt="$MEDUSA_BASE_OPT $MEDUSA_SYMB_OPT $MEDUSA_MEASURE_OPT"
        medusa_run_opt="$MEDUSA_BASE_OPT $MEDUSA_MEASURE_OPT"
        sliqsim_run_opt="$SLIQSIM_BASE_OPT $SLIQSIM_MEASURE_OPT"
        sliqsim_run_exec=$SLIQSIM_EXEC_MSR
    else
        benchmarks_fd=$BENCH_NO_MEASURE
        medusa_run_symb_opt="$MEDUSA_BASE_OPT $MEDUSA_SYMB_OPT"
        medusa_run_opt=$MEDUSA_BASE_OPT
        sliqsim_run_opt="$SLIQSIM_BASE_OPT $SLIQSIM_OPT"
        sliqsim_run_exec=$SLIQSIM_EXEC
    fi

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
                if [[ ( "$folder_name" = "Random" ) || ( "$folder_name" = "RevLib" ) ]]; then
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
    printf "${SEP}\n"
}

#####################################################################################
# Output:

printf "Without qubit measurement:\n\n"
is_measure=false # global so it can be read from all the functions
run_benchmarks

printf "\nWith qubit measurement:\n\n"
is_measure=true
run_benchmarks
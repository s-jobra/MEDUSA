#!/bin.bash
export LC_ALL=C.UTF-8

#exec settings
EXEC="sim"

#color settings
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
NC=$(tput sgr0)

SEP="================================================="

#print
printf "$SEP\n"
printf '%-20s %-15s %s\n' "File" "MySim" "SliQSim"
printf "$SEP\n"
for i in ../benchmarks/*/*/*.qasm; do
    FILE=${i#../benchmarks/}
    
    MY_TIME=$( timeout 2m ./$EXEC --time <$i 2>/dev/null | awk -F: '{gsub(/Time=/,""); printf "%.4fs \n", $1}')
    if [[ -z $MY_TIME ]]; then
        MY_TIME="${RED}Failed$NC"
    fi

    SLIQ_TIME="$( timeout 2m ./../SliQSim/SliQSim --sim_qasm $i --type 0 --shots 1024 --print_info 2>/dev/null | \
                  awk -F: '/Runtime:/{gsub(/ seconds/,""); printf "%.4fs \n", $2}')"
    SLIQ_TIME=${SLIQ_TIME% *}
    
    if [[ -z $SLIQ_TIME ]]; then
        SLIQ_TIME="${RED}Failed$NC"
        if [[ $MY_TIME != "Failed" ]]; then
            MY_TIME="${GREEN}${MY_TIME}$NC"
        fi
    else
        if [[ $MY_TIME == "${RED}Failed$NC" ]]; then
            SLIQ_TIME="${GREEN}${SLIQ_TIME}$NC"
        elif [[ $MY_TIME < "$SLIQ_TIME" ]]; then
            MY_TIME="${GREEN}${MY_TIME}$NC"
            SLIQ_TIME="${YELLOW}${SLIQ_TIME}$NC"
        else
            SLIQ_TIME="${GREEN}${SLIQ_TIME}$NC"
            MY_TIME="${YELLOW}${MY_TIME}$NC"
        fi
    fi

    printf '%-20s %-26s %s\n' "${FILE%/circuit*}" "$MY_TIME" "$SLIQ_TIME"
done
printf "${SEP}\n"
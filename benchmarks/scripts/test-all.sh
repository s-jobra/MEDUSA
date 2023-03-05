#!/bin/bash
export LC_ALL=C.UTF-8

# exec settings
EXEC="sim"

# measurement settings
REPS=10
TIMEOUT="timeout 10m" # for no timeout: ""

# output color settings
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
NC=$(tput sgr0)

SEP="========================================================================="

# print (body)
printf "$SEP\n"
printf '%-35s %-25s %s\n' "File" "MySim" "SliQSim"
printf "$SEP\n"
for i in ../benchmarks/*/*/*.qasm; do
    FILE=${i#../benchmarks/}
    
    MY_FAIL=false
    SLIQ_FAIL=false
    
    MY_TIME_SUM=0.0
    SLIQ_TIME_SUM=0.0

    for j in $(eval echo "{1..$REPS}"); do
        MY_TIME_CUR=$( $TIMEOUT ./$EXEC --time <$i 2>/dev/null | awk '{gsub(/Time=/,""); printf "%.4f \n", $1}')
        if [[ -z $MY_TIME_CUR ]]; then
            MY_TIME_AVG="${RED}Failed$NC"
            MY_FAIL=true
            break;
        else
            MY_TIME_SUM=$(echo "scale=4; $MY_TIME_SUM + $MY_TIME_CUR" | bc)
        fi
    done
    if [[ $MY_FAIL = false ]]; then
        MY_TIME_AVG=$(echo "scale=4; $MY_TIME_SUM / $REPS.0" | bc)
    fi

    for j in $(eval echo "{1..$REPS}"); do
        SLIQ_TIME_CUR="$( $TIMEOUT ./../SliQSim/SliQSim --sim_qasm $i --type 0 --shots 1024 --print_info 2>/dev/null | \
                  awk -F: '/Runtime:/{gsub(/ seconds/,""); printf "%.4fs \n", $2}')"
        SLIQ_TIME_CUR=${SLIQ_TIME_CUR%s*}
        if [[ -z $SLIQ_TIME_CUR ]]; then
            SLIQ_TIME_AVG="${RED}Failed$NC"
            SLIQ_FAIL=true
            break;
        else
            SLIQ_TIME_SUM=$(echo "scale=4; $SLIQ_TIME_SUM + $SLIQ_TIME_CUR" | bc)
        fi
    done
    if [[ $SLIQ_FAIL = false ]]; then
        SLIQ_TIME_AVG=$(echo "scale=4; $SLIQ_TIME_SUM / $REPS.0" | bc)
    fi

    if  [[ ( $MY_FAIL = true ) && ( $SLIQ_FAIL = false ) ]]; then
        SLIQ_TIME_AVG+="s"
        SLIQ_TIME_AVG="${GREEN}${SLIQ_TIME_AVG}$NC"
    elif [[ ( $MY_FAIL = false ) && ( $SLIQ_FAIL = true ) ]]; then
        MY_TIME_AVG+="s"
        MY_TIME_AVG="${GREEN}${MY_TIME_AVG}$NC"
    elif [[ ( $MY_FAIL = true ) && ( $SLIQ_FAIL = true ) ]]; then
        :
    else
        if (( $(echo "$MY_TIME_AVG < $SLIQ_TIME_AVG" |bc -l) )); then
            MY_TIME_AVG+="s"
            SLIQ_TIME_AVG+="s"
            MY_TIME_AVG="${GREEN}${MY_TIME_AVG}$NC"
            SLIQ_TIME_AVG="${YELLOW}${SLIQ_TIME_AVG}$NC"
        elif (( $(echo "$MY_TIME_AVG > $SLIQ_TIME_AVG" |bc -l) )); then
            MY_TIME_AVG+="s"
            SLIQ_TIME_AVG+="s"
            SLIQ_TIME_AVG="${GREEN}${SLIQ_TIME_AVG}$NC"
            MY_TIME_AVG="${YELLOW}${MY_TIME_AVG}$NC"
        else
            MY_TIME_AVG+="s"
            SLIQ_TIME_AVG+="s"
            SLIQ_TIME_AVG="${GREEN}${SLIQ_TIME_AVG}$NC"
            MY_TIME_AVG="${GREEN}${MY_TIME_AVG}$NC"
        fi
    fi

    printf '%-35s %-36s %s\n' "${FILE%/circuit*}" "$MY_TIME_AVG" "$SLIQ_TIME_AVG"
done
printf "${SEP}\n"
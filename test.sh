#!/bin.bash

#exec settings
EXEC="sim"

#color settings
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
NC=$(tput sgr0)

SEP="====================================="

#print
printf "\nTEST RESULTS:\n$SEP\n"
for i in ./examples/*.qasm; do
    FILE=${i#./examples/}
    ERROR="$(./$EXEC <$i 2>&1 >/dev/null)"
    if [[ -z $ERROR ]]; then
        #https://stackoverflow.com/questions/4617489/get-values-from-time-command-via-bash-script
        exec 4>&2
        TIME=$( { time ./$EXEC <$i 1>/dev/null 2>&4; } 2>&1 | awk -F: '/real/{gsub(/real[ \t]*/,"");print $1}')
        exec 4>&-
        TIME="${GREEN}${TIME}$NC"
    else
        TIME="${RED}Failed$NC"
    fi
    
    SUMMARY+=$(printf '%-20s %s' "${FILE%.*}" "$TIME")
    SUMMARY+="\n"

done
printf "${SUMMARY}${SEP}\n"
#!/bin.bash

IN_FILE=test_new.out
OUT_FILE=test_res.svg

RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
NC=$(tput sgr0)

awk \
        '/^BernsteinVazirani/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2=0; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3=0; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > bv-temp.out

awk \
        '/^Feynman/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2=0; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3=0; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > feynman-temp.out

awk \
        '/^Grover/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2=0; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3=0; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > grover-temp.out

awk \
        '/^MCToffoli/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2=0; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3=0; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > mctoffoli-temp.out

awk \
        '/^MOGrover/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2=0; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3=0; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > mogrover-temp.out

awk \
        '/^Random/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2=0; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3=0; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > random-temp.out

awk \
        '/^RevLib/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2=0; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3=0; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > revlib-temp.out

gnuplot -persist <<-EOFMarker
    set terminal svg size 1920,1080 enhanced background rgb 'white'
    set output "$OUT_FILE"
    
    set multiplot

    set origin 0,0
    set size 1,1
    set xlabel "SliQSim Time (s)"
    set ylabel "MySim Time (s)"
    set grid
    set xrange [0:0.7]
    set yrange [0:0.7]
    set xtics 0.05
    set ytics 0.05
    plot "bv-temp.out" title "BV" lt 4 lc 2,\
         "feynman-temp.out" title "Feynman" lt 4 lc 3,\
         "grover-temp.out" title "Grover" lt 4 lc 4,\
         "mctoffoli-temp.out" title "MCToffoli" lt 4 lc 5,\
         "mogrover-temp.out" title "MOGrover" lt 4 lc 6,\
         "random-temp.out" title "Random" lt 4 lc 8,\
         "revlib-temp.out" title "RevLib" lt 4 lc 9,\
         x lc 7 title ""

    set origin 0.5,0.05
    set size 0.49,0.49
    set xrange [0:0.07]
    set yrange [0:0.07]
    set xtics 0.01
    set ytics 0.01
    set xlabel ""
    set ylabel ""
    unset key
    unset grid
    replot
EOFMarker

rm -f *-temp.out
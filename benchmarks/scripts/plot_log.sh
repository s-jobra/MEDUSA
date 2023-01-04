#!/bin.bash

IN_FILE=../results/benchmark2.out
OUT_FILE=../benchmark2_log.svg

RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
NC=$(tput sgr0)

awk \
        '/^BernsteinVazirani/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2="0.0001"; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3="0.0001"; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > bv-temp.out

awk \
        '/^Feynman/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2="0.0001"; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3="0.0001"; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > feynman-temp.out

awk \
        '/^Grover/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2="0.0001"; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3="0.0001"; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > grover-temp.out

awk \
        '/^MCToffoli/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2="0.0001"; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3="0.0001"; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > mctoffoli-temp.out

awk \
        '/^MOGrover/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2="0.0001"; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3="0.0001"; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > mogrover-temp.out

awk \
        '/^Random/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2="0.0001"; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3="0.0001"; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > random-temp.out

awk \
        '/^RevLib/ { if (match($2, /[0-9]*\.[0-9]*/)) { $2=substr($2, RSTART, RLENGTH); my_fail=0;} else { $2="0.0001"; my_fail=1;}; \
                                if (match($3, /[0-9]*\.[0-9]*/)) { $3=substr($3, RSTART, RLENGTH); sliq_fail=0;} else { $3="0.0001"; sliq_fail=1; }; \
                                if (my_fail||sliq_fail) {t = $2; $2 = $3; $3 = t;}; \
                                print $3, $2}' $IN_FILE > revlib-temp.out

gnuplot -persist <<-EOFMarker
    set terminal svg size 1920,1080 enhanced background rgb 'white'
    set output "$OUT_FILE"
    
    set multiplot

    set origin 0,0
    set size 1,1
    set xlabel "SliQSim Time (s)" font ",20"
    set ylabel "MySim Time (s)" font ",20"
    set logscale
    set grid

    set xrange [0.0001:1000]
    set yrange [0.0001:1000]
    set size square

    set xtics font ",20"
    set ytics font ",20"
    set key left top font ",20" box
    plot "bv-temp.out" title "BV" pt 6 lc 9 ps 1.5,\
         "feynman-temp.out" title "Feynman" pt 7 lc 4 ps 1.5,\
         "grover-temp.out" title "Grover" pt 5 lc 3 ps 1.5,\
         "mctoffoli-temp.out" title "MCToffoli" pt 4 lc 8 ps 1.5,\
         "mogrover-temp.out" title "MOGrover" pt 3 lc 9 ps 1.5,\
         "random-temp.out" title "Random" pt 2 lc 2 ps 1.5,\
         "revlib-temp.out" title "RevLib" pt 10 lc 6 ps 1.5,\
         x lc 7 title ""
EOFMarker

rm -f *-temp.out
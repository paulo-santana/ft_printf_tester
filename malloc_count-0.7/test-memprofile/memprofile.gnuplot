#!/usr/bin/env gnuplot

set terminal pdf size 28cm,18cm linewidth 2.0
set output "memprofile.pdf"

set key top right
set grid xtics ytics

set title 'Memory Profile of Test Program'
set xlabel 'Time [s]'
set ylabel 'Memory Usage [MiB]'

plot \
    'memprofile.txt' using 1:($2 / 1024/1024) title 'memprofile' with lines

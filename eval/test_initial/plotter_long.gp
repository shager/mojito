#!/usr/bin/gnuplot

reset

#set terminal wxt size 350,262 enhanced font 'Verdana,10' persist
# png
#set terminal pngcairo size 700,560 enhanced font 'Verdana,10'
#set output 'test_initial_long.png'
# svg
set terminal svg size 700,500 fname 'Verdana, Helvetica, Arial, sans-serif' fsize '10'
set output 'test_initial_long.svg'

# color definitions
set border linewidth 1.5

set style line 1 lc rgb '#0060ad' lt 1 lw 0.5 pt 7 ps 0.5 # blue

set title 'UDP datarate'

set xlabel 'Number of flowtable entries' 
set ylabel 'Maximum throughput (MBit/s)'

set xrange [-5:1000]

plot 'test_initial_long.dat.mean' using 1:2:3 with yerrorbars notitle ls 1, '' using 1:2 w linespoints ls 1 notitle

